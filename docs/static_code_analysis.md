Static code analysis  {#static_code_analysis}
====================

We use [SonarQube](https://www.sonarqube.org/) for static code analysis scans.
These are automated via Travis, same as our usual builds.

Automated scans
---------------

We don't use the Travis' plugin for Sonar due to the fact we use Docker
and not the bare Travis, and these two are not compatible.

We have a dedicated `docker-compose` target for scans, `sonar-scan`. Necessary
values are passed to Sonar scanner as command-line parameters.

For the whole config to work, the following one-time configuration steps are necessary:
* Create organization and project in SonarQube - done already,
   https://sonarcloud.io/organizations/mraa-github (key: mraa-github)
   and https://sonarcloud.io/dashboard?id=mraa-master (key: mraa-master);
* Create technical account on GitHub with push permissions for mraa repo.
   It is used for reporting pull request statuses in the "checks" area.
   We have `intel-iot-devkit-helperbot` for this, shared with UPM.
* Add several environment variables in Travis:
    * `GITHUB_TOKEN` (secure) - GH OAuth token for the technical user,
       with `public_repo` privileges only;
    * `SONAR_TOKEN` (secure) - this one comes from the SonarQube org properties;
    * `SONAR_ORG`, `SONAR_PROJ_KEY` (may be public) - project and org keys (names)
       from SonarQube org, see above;

These scans are executed each time there's an internal pull request (from a branch
local to main mraa repo) or a `master` branch push.
Upon the former the so called "preview" scan is executed, which doesn't
upload anything to SonarQube organization and only reports the result within the PR.
Upon `master` branch push a normal scan is executed and results are uploaded to
SonarQube.

When there's a so called "external" pull request (originating somewhere else
than mraa's main repo, e.g. from a fork), no scan is done for
security reasons, as code within such PR would have access to tokens listed above.

In view of such setup, it's beneficial to create internal pull requests as much
as possible, because you'll catch problems right away - in the preview scan,
before PR is merged.

Manual scans
------------

It's a good practice to run the scan manually before actually submitting a PR.
There may also be a need to run the scan manually out-of-cycle, so here's how.

Just use the command line from [the scanner script](../master/scripts/sonar-scan.sh).
See `sonar_cmd_base` variable specifically and just replace various tokens
listed there with proper ones. Please also don't forget that you need to run the
build wrapper first, so that the scanner knows what to scan.

The set of commands for the main mraa repo and SonarQube project would look like
the below. Note that it will upload results to the SonarQube by default,
if you don't want that, setup a throwaway "project" in SonarQube, or create a
separate "organization" dedicated to your mraa repo fork:

```bash
$> export PATH=~/bin/sonar/sonar-scanner-3.0.3.778-linux/bin/:~/bin/sonar/build-wrapper-linux-x86/:$PATH
$> build-wrapper-linux-x86-64 --out-dir bw-output make clean all
$> sonar-scanner \
    -Dsonar.projectKey=mraa-master \
    -Dsonar.projectBaseDir=/PATH/TO/YOUR/MRAA/REPO/CLONE \
    -Dsonar.sources=/PATH/TO/YOUR/MRAA/REPO/CLONE \
    -Dsonar.inclusions='api/**/*,CMakeLists.txt,examples/**/*,imraa/**/*,include/**/*,src/*,src/**/*,tests/**/*' \
    -Dsonar.cfamily.build-wrapper-output=/PATH/TO/YOUR/MRAA/REPO/CLONE/YOUR_BUILD_DIR/bw-output \
    -Dsonar.host.url=https://sonarcloud.io \
    -Dsonar.organization=mraa-github \
    -Dsonar.login=YOUR_SONAR_LOGIN_TOKEN_HERE
```

Notice that we first set the `PATH` to point to our downloaded copy of Sonar tools.
You can find more information on setting these up in SonarQube's nice
[Getting Started tutorial](https://about.sonarcloud.io/get-started/).

Using Coverity
--------------

In the past we've used Coverity to do static code analysis scans. Below is the
documentation on that setup - for archiving purposes.

This is the procedure to submit a build to Coverity. You'll need to install
`coverity-submit` for your OS.

```bash
mkdir covbuild/ && cd covbuild
cmake -DBUILDDOC=OFF -DBUILDSWIG=OFF ..
cov-build --dir cov-int make
tar caf mraa.tar.bz2 cov-int
```
