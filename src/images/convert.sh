#! /bin/sh

if [ -z $1 ]; then
  echo "Usage: $0 filename (WITHOUT extension)";
  exit;
fi

gdk-pixbuf-csource --raw --name=$1 $1.png > $1.h
