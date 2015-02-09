Debugging libmraa                         {#debugging}
=================

This page contains a few debugging tip. When filing an issue please go through
this list as it's the first thing we'll ask you to do.

### Finding your mraa version

Getting the exact version of libmraa you're running is really important to us.
The best way to get this is to call mraa_get_version() or mraa.getVersion(). If
mraa returns x.x.x-dirty then your version was not built from a git tree or you
built out of tree (see our building doc) - or you don't have git installed.

### Finding error logs

mraa logs pretty much everything that goes wrong to syslog. These days it'll go
to the systemd journal so check with `sudo journalctl -f` whilst running your
app or check all libmraa messages with `journalctl -t libmraa`.  Grab all the
messages from the initialisation message right up to your last one. Using a
pastebin is always a good idea, I like dpaste.com. If your system does not have
systemd likely your log is in /var/log/messages or a similar location.

### Common errors to check for

* Not running as root
* Incorrect IO pin numbers, mraa uses physical connector pin numbering see your
  platform documentation for details
* Your platform is unsupported
* Using the wrong pin, check pin capabilities either using the API or your
  platform documentation

