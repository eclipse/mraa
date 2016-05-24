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
