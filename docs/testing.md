# Testing mraa

Mraa has a test suite written in python which can be run by users to check
their platform is working correctly. The aim is to have a full API coverage
using python scripts using unittests. Currently the test suite is far from
complete.

## Supported platforms

Intel Galileo v1 was used and is currently the only supported platform. Tests
were conducted on Intel Galileo v2, but because of hardware differences, some
failures may occur. Logic will be added to auto-select board to work on. To
start with boards with an arduino type shield/layout should work

## How to run these tests

You need to compile from source with -DBUILDSWIGPYTHON=ON and then tests can be
executed with:
$ make test

More detail can be seen by running:
$ ctest -VV

Note tests will not run on platforms which cannot initialise, checking the
amount of 'skipped' tests can be useful

## What's next?

At this point tests were made to do a quick sanity check. In the future the
followings will be implemented:
 * Create a physical layer that will emulate sensors behavior and generate I/O
   signals. This layer will be available to the community and will help replace
   the need of specific sensors. The simplest solution will be chosen allowing
   anyone to rapidly implement it.
 * Implement stress tests over long periods of time. This feature alone is
   possible only with the use of a physical layer since it will behave both as
   signals generator and interpreter.
 * Analyze a tests result collection solution (and after, interpretation). This
   is useful since tests mean nothing without a history and possibility to
   check on results.
 * Any idea welcome!

