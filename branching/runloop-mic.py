#! /usr/bin/env python

import os;

ivals = range(10);

for i in ivals:
  val= i/10.;
  cmd_fmt = "ssh mic0 ./bb2-mic %f > dump_mic_%f.dat";
  cmd = cmd_fmt % (  val, val );
  print "running command >", cmd, "<";
  os.system(cmd);


for i in ivals:
  val= (i+1)/50.;
  cmd_fmt = "ssh mic0 ./bb2-mic %f > dump_mic_%f.dat";
  cmd = cmd_fmt % (  val, val );
  print "running command >", cmd, "<";
  os.system(cmd);
