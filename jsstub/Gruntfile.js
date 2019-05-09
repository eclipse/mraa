/*
 * Author: David Antler <david.a.antler@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

module.exports = function(grunt) {
  grunt.loadNpmTasks('grunt-mocha-test');
  // Project configuration.
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    mochaTest: {
      test: {
        options: {
          reporter: 'spec'
        },
        src: ['test/*.js']
      }
    }
  });

  grunt.registerTask('test', ['mochaTest:test']);
};
