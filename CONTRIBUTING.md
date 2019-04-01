Contributing to Eclipse Mraa (libmraa)                   {#contributing}
======================

Eclipse Mraa is an opensource project and we are actively looking for people to help
with:

- Writing platform supports for all types of boards running linux
- People to extend the functionality, API with useful functions
- Anything we haven't thought about :) Ideas always welcome!

The recommended method to contribute is to fork on github, and then send pull
requests to the main project. You can open issues if you find any bugs/have
questions. If you want to work on a large feature then we suggest you file an
issue first so we can avoid dissapointments come merging time!

If you'd rather not use github you are more than welcome to send git formatted
patches to our mailing list mraa@lists.01.org which you can register for access
on: https://lists.01.org/mailman/listinfo/mraa

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

Author Rules
------------

If you create a file, then add yourself as the Author at the top. If you did a
large contribution to it (or if you want to ;-)), then fee free to add yourself
to the contributors list in that file. You can also add your own copyright
statement to the file but cannot add a license of your own. If you're borrowing
code that comes from a project with another license, make sure to explicitly
note this in your PR.

Eclipse Contributor Agreement
------------

Your contribution cannot be accepted unless you have a signed [ECA - Eclipse Foundation Contributor Agreement](http://www.eclipse.org/legal/ECA.php) in place.

Here is the checklist for contributions to be _acceptable_:

1. [Create an account at Eclipse](https://dev.eclipse.org/site_login/createaccount.php).
2. Add your GitHub user name in your account settings.
3. [Log into the project's portal](https://projects.eclipse.org/) and sign the ["Eclipse ECA"](https://projects.eclipse.org/user/sign/cla).
4. Ensure that you [_sign-off_](https://wiki.eclipse.org/Development_Resources/Contributing_via_Git#Signing_off_on_a_commit) your Git commits.
5. Ensure that you use the _same_ email address as your Eclipse account in commits.
6. Include the appropriate copyright notice and license at the top of each file.

Your signing of the ECA will be verified by a webservice called 'ip-validation'
that checks the email address that signed-off on your commits has signed the
ECA. **Note**: This service is case-sensitive, so ensure the email that signed
the ECA and that signed-off on your commits is the same, down to the case.

Where to find us
----------------

Hop onto the freenode network on IRC and join #mraa. Please be patient as we're
not always online.

