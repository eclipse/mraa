platform-hooks                             {#hooks}
==============

Hooks can be defined per supported platform to allow for highly custom
operations if needed.  This feature of MRAA should only be used by developers
defining the board definitions, NOT an end user.

##Types of Hooks

###REPLACE
Defining a replace function will entirely replace the associate function. This
should only be done if your new function can handle everything the mraa
function would normally.

###PRE
Any functionality defined here will be performed when the main function is
called.

###POST
Any functionality perfomed here is done just before the normal function
returns. All post functions will have passed into them the return value that
would normally be returned.

##Hooks
### GPIO
 * init (pre-post)
 * mode (replace-pre-post)
 * dir (replace-pre-post)
 * write (pre-post)
 * use-mmaped (replace-pre-post)

### I2C
 * init (pre-post) - On RAW
