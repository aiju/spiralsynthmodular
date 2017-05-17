****** SpiralSynthModular ******

Last changed Tuesday 6-January-2004.

SpiralSynthModular is open source software, distributed under the General
Public License (GPL). See the file COPYING.

See www.pawfal.org or sourceforge.net/projects/spiralmodular for more details.


*** Warning ***

If you are using GCC version 3, you should note that the libstdc++ included with
the compiler uses code from the system C library (glibc) version 2.2.5. That
version of glibc is over a year old and contains necessary bugfixes. Some people
have reported that their distro. contains an older version, which leads to SSM
crashing with a segfault almost immediately.


*** Dependancies ***

FLTK (www.fltk.org)
If you build FLTK from source make sure you use "./configure --enable-shared"

LADSPA plugins  (www.ladspa.org)

Sound output can be provided by either OSS or JACK (and therefore alsa).


*** Installing ***

To build the SpiralSynthModular application, and all the plugins:

./configure
make
make install
spiralsynthmodular

Check out the designs in the Examples directory.
For more examples be sure to check out the noize farm
(http://groups.yahoo.com/group/noize_farm) an open source repository of
ssm patches.

usage:
spiralsynthmodular options [patch.ssm]

options list:
-h : help
-v : print version
--NoGUI : run without GUI (only useful when loading patch from command line
--Realtime : spawn audio thread with FIFO scheduling (run as root)
--PluginPath <PATH> : look for plugins in the specified directory


*** LRDF support for LADSPA plugins ***

http://plugin.org.uk/releases/lrdf/
http://www.redland.opensource.ac.uk/raptor/

If you have the lrdf library installed on your system,
the LADSPAPlugin will automatically use it.

To disable detection of this this, use the configure option:
./configure --disable-liblrdf


*** Jack support ***

http://jackit.sourceforge.net

If you have jack installed on your system, you can use it via the
JackPlugin.

To load the plugin, add it to .SpiralSynthModular's plugin list after
installation.

To disable detection of jack, use the configure option:
./configure --disable-jack


*** LibSndFile Support ***

http://www.zip.com.au/~erikd/libsndfile/

If you have libsndfile installed on you system; PoshSampler, Streamer,
and SpiralLoops load any audio format/file that libsndfile can, though
the Open Dialog Filter currently doesn't reflect this.

To disable detection of this, use the configure option:
./configure --disable-sndfile


*** Setting up your .spiralmodular file ***

This file contains all the setup variables for the program. Thes values can be
edited with the options window whilst running SSM.
The following variables in the are the ones to play with to get a good playback
on your machine. This is usually tradeoff between quality and responsiveness
(latency). You can build designs and songs at low quality (22050 samplerate etc)
and then record them at higher settings. The wav file will sound fine, even if
the realtime output doesn't.

BufferSize        = 512         - Size of sample buffer to be calculated each cycle.
FragmentSize      = 256         - Subbuffer size sent to the soundcard
FragmentCount     = 8           - Subbuffer count, -1 computes a suitable size,
                                  setting to 4 or 8 gets better latencies.
Samplerate        = 44100       - Sets the samplerate


*** Trying it out without installing ***

You can try SpiralSynthModular out by following the following steps.

1. Run the script ./createlinks to create a dummy plugin directory in your source
   directory

2. To run use the command:
   ./spiralsynthmodular --PluginPath /path/to/your/ssm/source/links


Disclaimer:
No representations are made about the suitability of this software
for any purpose. This software is provided "as is" without express
or implied warranty.
(Damage to ears, speakers or buildings are not my fault :))

Dave Griffiths (Headmaster) - dave@pawfal.org
Andy Preston (Caretaker) - andy@clubunix.co.uk

www.pawfal.org
