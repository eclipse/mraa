Contributing to libmraa                           {#contributing} 
======================

libmraa is an opensource project and we are actively looking for people to help
with:

- Writing platform supports for all types of boards running linux
- People to extend the functionality, API with useful functions
- Anything we haven't thought about :) Ideas always welcome!

The recommended method to contribute is to fork on github, and then send pull
requests to the main project. You can open issues if you find any bugs/have
questions.

If you'd rather not use github you are more than welcome to send git formatted
patches to brendan.le.foll@intel.com.

Basic rules
-----------
- Your code must build
- Commits must have a sign-off line by at least yourself
- Commits must be named <file/module>: Some decent description
- Try not to break master. In any commit.
- Try to split commits up logically, you will be asked to rebase them if they
  are not.
- Try to stick to the established coding style regardless of your personal
  feeling for it! Use clang-format (3.6+ required)

Coding Style
------------

Coding style for all code is defined by clang-format, have a look at it. Avoid
styling fixes as they make history difficult to read. Javascript & Java can
also be parsed through the clang-format, it complains but seems to do an ok
job. Few exceptions to coding styles:
- All python code is indented by 2 spaces
- CmakeLists files are 2 space indented and a space is required before all
  brackets so endif () and if () and command (). Also use lowercase for
  everything but variables. Cmake is case insensitive but this isn't the wild
  wild west ;-)

Use common sense and don't be afraid to challenge something if it doesn't make sense!

Code signing
------------

The sign-off is a simple line at the end of the explanation for the
patch, which certifies that you wrote it or otherwise have the right to pass it
on as an open-source patch.  The rules are pretty simple: if you can certify
the below:

        Developer's Certificate of Origin 1.1

        By making a contribution to this project, I certify that:

        (a) The contribution was created in whole or in part by me and I
            have the right to submit it under the open source license
            indicated in the file; or

        (b) The contribution is based upon previous work that, to the best
            of my knowledge, is covered under an appropriate open source
            license and I have the right under that license to submit that
            work with modifications, whether created in whole or in part
            by me, under the same open source license (unless I am
            permitted to submit under a different license), as indicated
            in the file; or

        (c) The contribution was provided directly to me by some other
            person who certified (a), (b) or (c) and I have not modified
            it.

        (d) I understand and agree that this project and the contribution
            are public and that a record of the contribution (including all
            personal information I submit with it, including my sign-off) is
            maintained indefinitely and may be redistributed consistent with
            this project or the open source license(s) involved.

then you just add a line saying

	Signed-off-by: Random J Developer <random@developer.example.org>

Using your real name (sorry, no pseudonyms or anonymous contributions.)

Where to find us
----------------

Hop onto the freenode network on IRC and join #mraa. Please be patient as we're
not always online.

