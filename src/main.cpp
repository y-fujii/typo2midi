// (c) Yasuhiro Fujii <http://mimosa-pudica.net>, under MIT License.

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <linux/input.h>
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
			throw exception();
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

int main(int argc, char const* const* argv) {
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <input_device_path>" << endl;
		return -1;
	}

	jack_ringbuffer_t* buffer = jack_ringbuffer_create(4096);
	try {
		JackMidiWriter writer(buffer, argv[0]);
		read_keys(buffer, argv[1], 63);
	}
	catch (...) {
		jack_ringbuffer_free(buffer);
		throw;
	}
	return 0;
}
