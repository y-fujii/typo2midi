// (c) Yasuhiro Fujii <http://mimosa-pudica.net>, under MIT License.

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <libinput.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

using namespace std;


struct JackMidiWriter {
	~JackMidiWriter() {
		jack_client_close(_jack);
	}

	JackMidiWriter(jack_ringbuffer_t* buffer, char const* name):
		_buffer(buffer)
	{
		_jack = jack_client_open(name, JackNullOption, nullptr);
		if (_jack == nullptr) {
			throw runtime_error("failed to initialize JACK.");
		}
		_port = jack_port_register(_jack, "out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
		jack_set_process_callback(_jack, _process, this);
		jack_activate(_jack);
	}

private:
	static int _process(jack_nframes_t size, void* self_v) {
		JackMidiWriter* self = reinterpret_cast<JackMidiWriter*>(self_v);

		void* buf = jack_port_get_buffer(self->_port, size);
		jack_midi_clear_buffer(buf);

		while (jack_ringbuffer_read_space(self->_buffer) >= 3) {
			array<uint8_t, 3> msg;
			jack_ringbuffer_read(self->_buffer, reinterpret_cast<char*>(msg.data()), msg.size());
			jack_midi_event_write(buf, 0, msg.data(), msg.size());
		}

		return 0;
	}

	jack_ringbuffer_t* _buffer;
	jack_client_t* _jack;
	jack_port_t* _port = nullptr;
};

// ref. <https://github.com/wayland-project/libinput/blob/master/tools/libinput-debug-events.c>.
// ref. <https://github.com/wayland-project/libinput/blob/master/tools/shared.c>.
struct LibInputReader {
	~LibInputReader() {
		libinput_unref(_libinput);
	}

	LibInputReader() {
		udev* udev = udev_new();
		if (udev == nullptr) {
			throw runtime_error("failed to initialize udev.");
		}
		_interface.open_restricted = _open_restricted;
		_interface.close_restricted = _close_restricted;
		_libinput = libinput_udev_create_context(&_interface, nullptr, udev);
		if (_libinput == nullptr) {
			udev_unref(udev);
			throw runtime_error("failed to initialize libinput.");
		}
		libinput_udev_assign_seat(_libinput, "seat0");
		udev_unref(udev);
	}

	void run(jack_ringbuffer_t* buffer, uint8_t vel) {
		pollfd fds;
		fds.fd = libinput_get_fd(_libinput);
		fds.events = POLLIN;
		do {
			libinput_dispatch(_libinput);
			while (libinput_event *ev = libinput_get_event(_libinput)) {
				if (libinput_event_get_type(ev) == LIBINPUT_EVENT_KEYBOARD_KEY) {
					libinput_event_keyboard* evk = libinput_event_get_keyboard_event(ev);
					uint32_t key = libinput_event_keyboard_get_key(evk);
					if (key < 128) {
						libinput_key_state state = libinput_event_keyboard_get_key_state(evk);
						array<char, 3> msg;
						msg[0] = state == LIBINPUT_KEY_STATE_RELEASED ? 0x80 : 0x90;
						msg[1] = key;
						msg[2] = vel;
						jack_ringbuffer_write(buffer, msg.data(), msg.size());
					}
				}
				libinput_event_destroy(ev);
			}
		} while (poll(&fds, 1, -1) >= 0);
	}

private:
	static int _open_restricted(char const* path, int flags, void*) {
		int result = open(path, flags);
		if (result < 0) {
			// XXX
			throw runtime_error("failed to open libinput device.");
		}
		return result;
	}

	static void _close_restricted(int fd, void*) {
		close(fd);
	}

	libinput_interface _interface;
	libinput* _libinput = nullptr;
};

#if 0
void read_keys(jack_ringbuffer_t* buffer, char const* path, uint8_t vel) {
	ifstream dev(path);
	dev.exceptions(ifstream::badbit);
	input_event ev;
	while (dev.read(reinterpret_cast<char*>(&ev), sizeof(ev))) {
		if (ev.type != EV_KEY || ev.value >= 2 || ev.code >= 128) {
			continue;
		}
		array<char, 3> msg;
		msg[0] = ev.value == 0 ? 0x80 : 0x90;
		msg[1] = ev.code;
		msg[2] = vel;
		jack_ringbuffer_write(buffer, msg.data(), msg.size());
	}
}
#endif

int main(int argc, char const* const* argv) {
	if (argc != 1) {
		cerr << "Usage: " << argv[0] << endl;
		return -1;
	}

	jack_ringbuffer_t* buffer = jack_ringbuffer_create(4096);
	try {
		JackMidiWriter writer(buffer, argv[0]);
		LibInputReader().run(buffer, 63);
	}
	catch (...) {
		jack_ringbuffer_free(buffer);
		throw;
	}

	return 0;
}
