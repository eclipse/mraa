Debugging libmraa                         {#debugging}
=================

This page contains a few debugging tip. When filing an issue please go through
this list as it's the first thing we'll ask you to do.

### Finding your mraa version

Getting the exact version of libmraa you're running is really important to us.
The best way to get this is to call mraa_get_version() or mraa.getVersion(). If
you built from git then please tell us which commit hash you used. Mraa no
longer tags itself as 'dirty' if built as a non tagged version but simply uses
the latest version number listed in the top level CMakeLists.txt file. Note
that you may have to wipe the build/ dir for git sha version to be absolutely
correct.

### Finding error logs

mraa logs pretty much everything that goes wrong to syslog. These days it'll go
to the systemd journal so check with `sudo journalctl -f` whilst running your
app or check all libmraa messages with `journalctl -t libmraa`.  Grab all the
messages from the initialisation message right up to your last one. Using a
pastebin is always a good idea, I like dpaste.com. If your system does not have
systemd likely your log is in /var/log/messages or a similar location.

### Common errors to check for

* Not running as a user with permissions to the physical char devices/files
  that require access. Check quickly if it works as root and you likely have
  such an issue. The log file will reflect this
* Incorrect IO pin numbers, mraa uses physical connector pin numbering see your
  platform documentation for details
* Your platform is unsupported or has been detected wrongly (logfile will show this)
* Using the wrong pin, check pin capabilities either using the API or your
  platform documentation

