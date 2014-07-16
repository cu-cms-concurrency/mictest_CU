#! /usr/bin/env python

import os;

ivals = range(10)


for i in ivals:
  val= i/10.;
  cmd_fmt = "./bb2 %f > dump_%f.dat";
  cmd = cmd_fmt % (  val, val );
  print "running command >", cmd, "<";
  os.system(cmd);

for i in ivals:
  val= (i+1)/50.;
  cmd_fmt = "./bb2 %f > dump_%f.dat";
  cmd = cmd_fmt % (  val, val );
  print "running command >", cmd, "<";
  os.system(cmd);

