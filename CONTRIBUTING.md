Contributing to Eclipse Mraa (libmraa)                   {#contributing}
======================

Eclipse Mraa is an open source project and we are actively looking for people to help
with:

- Writing platform supports for all types of boards running linux
- People to extend the functionality, API with useful functions
- Anything we haven't thought about :) Ideas always welcome!

The accepted method to contribute is to fork the project on Github and open
pull requests with your changes. You can open issues if you find any bugs/have
questions. If you want to work on a large feature then we suggest you file an
issue first so we can avoid disappointments come merging time!

Basic rules
-----------
- Your code must build
- Commits must be named <file/module>: Some decent description
- Try not to break master. In any commit.
- Try to split commits up logically, you will be asked to rebase them if they
  are not.
- Try to stick to the established coding style regardless of your personal
  feeling for it! Use clang-format (3.6+ required)

Coding Style
------------

Coding style for all code is defined by clang-format, have a look at it. Avoid
styling fixes as they make history difficult to read. JavaScript & Java can
also be parsed through the clang-format, it complains but seems to do an ok
job. Few exceptions to coding styles:
- All Python code is indented by 2 spaces
- CMakeLists files are 2 space indented and a space is required before all
  brackets so `endif ()` and `if ()` and `command ()`. Also use lowercase for
  everything but variables. CMake is case insensitive but this isn't the wild
  wild west ;-)

Use common sense and don't be afraid to challenge something if it doesn't make sense!

Author Rules
------------

If you create a file, then add yourself as the Author at the top. If you did a
large contribution to it (or if you want to ;-)), then feel free to add yourself
to the contributors list in that file. You can also add your own copyright
statement to the file but cannot add a license of your own. If you're borrowing
code that comes from a project with another license, make sure to explicitly
note this in your PR.

Eclipse Contributor Agreement
------------

Your contribution cannot be accepted unless you have a signed [ECA - Eclipse Foundation Contributor Agreement](https://www.eclipse.org/legal/eca/) in place.

Here is the checklist for contributions to be _acceptable_:

1. [Create an account at Eclipse](https://accounts.eclipse.org/user/register).
2. Add your GitHub user name in your account settings.
3. [Log in to the projects forge](https://www.eclipse.org/contribute/cla/) and sign the ["Eclipse ECA"](https://accounts.eclipse.org/user/eca).
5. Ensure that you use the _same_ email address as your Eclipse account in commits.
6. Include the appropriate copyright notice and license at the top of each file.

Your signing of the ECA will be verified by a Github Check called 'eclipsefdn/eca'
that checks the email address that authored your commits has signed the
ECA. **Note**: This service is case-sensitive, so ensure the email that signed
the ECA and that authored your commits is the same, down to the case.

Where to find us
----------------

Hop onto the freenode network on IRC and join #mraa. Please be patient as we're
not always online.
