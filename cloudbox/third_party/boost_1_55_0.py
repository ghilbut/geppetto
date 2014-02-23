#!/usr/bin/python
# -*- coding:utf-8 -*-

import os
import zipfile
import threading
import urllib2
from subprocess import call

is_win = (os.name == 'nt')

rootpath = os.path.abspath(os.path.dirname(__file__))



class ArchiveReadyThread(threading.Thread):

  def __init__(self, num, params):
    threading.Thread.__init__(self)
    self.num = num
    self.basepath = os.path.join(rootpath, 'tmp')
    if not os.path.exists(self.basepath):
      os.makedirs(self.basepath)
    self.url     = params['url']
    self.success = params['success']
    self.failed  = params['failed']

  def download(self):
    num = self.num
    url = self.url
    basepath = self.basepath

    print '%d> download: %s' % (num, url)
    name = os.path.split(url)[1]
    path = os.path.join(basepath, name)
    response = urllib2.urlopen(url)
    data = response.read()
    tgz = open(path, "wb")
    tgz.write(data)
    tgz.close()
    return path

  def run(self):
    filepath = self.download()
    self.success(self.num, filepath)



def failed(num, path):
  print  ('%d> failed') % num



def ready_7zip(filepath):
  bin = os.path.join(rootpath, 'tmp')
  print filepath
  with zipfile.ZipFile(filepath, 'r') as zip:
    zip.extractall(bin)
  os.environ['PATH'] += (os.pathsep + bin)

t7zip = {
  'url': 'http://downloads.sourceforge.net/sevenzip/7za920.zip',
  'success': (lambda num, path: ready_7zip(path)),
  'failed':  (lambda num, path: failed(num, path))
}

boost = {
  'url': 'http://jaist.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.%s' % ('7z' if is_win else 'tar.gz'),
  'success': (lambda num, path: None),
  'failed':  (lambda num, path: failed(num, path))
}

bzip2 = {
  'url': 'http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz',
  'success': (lambda num, path: None),
  'failed':  (lambda num, path: failed(num, path))
}

icu4c = {
  'url': 'http://download.icu-project.org/files/icu4c/51.2/icu4c-51_2-src.tgz',
  'success': (lambda num, path: None),
  'failed':  (lambda num, path: failed(num, path))
}

python27 = {
  'url': 'http://python.org/ftp/python/2.7.6/Python-2.7.6.tgz',
  'success': (lambda num, path: None),
  'failed':  (lambda num, path: failed(num, path))
}

zlib = {
  'url': 'http://zlib.net/zlib-1.2.8.tar.gz',
  'success': (lambda num, path: None),
  'failed':  (lambda num, path: failed(num, path))
}

threads = [
  ArchiveReadyThread(0, t7zip),
  ArchiveReadyThread(1, boost),
  ArchiveReadyThread(2, bzip2),
  ArchiveReadyThread(3, icu4c),
  ArchiveReadyThread(4, python27),
  ArchiveReadyThread(5, zlib)
]

for thread in threads:
  thread.start()
for thread in threads:
  thread.join()



if is_win:
  os.chdir(rootpath)
  call(os.path.join(rootpath, 'boost_1_55_0.bat'), shell=True)


#import shutil
#shutil.rmtree(os.path.abspath(os.path.join(rootpath, 'tmp')))