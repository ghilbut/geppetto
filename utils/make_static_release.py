# -*- coding: utf-8 -*-

import os
import sys

import boto
from boto.s3.key import Key



path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../'))
if path not in sys.path:
    sys.path.append(path)

# Djnago - ntools
os.environ['DJANGO_SETTINGS_MODULE'] = 'ntools.settings'

# Amazon Cloud Sevice - S3
from ntools.settings import AWS_PUBLIC_KEY, AWS_SECRET_KEY
s3 = boto.connect_s3(AWS_PUBLIC_KEY, AWS_SECRET_KEY)
b = s3.get_bucket('storage.n-tools.net')
k = Key(b)



# function to compress, encode and upload
def make_file(base, filelist, s3_key):
    import codecs;
    script = u''.encode('utf-8')
    for name in filelist:
        filepath = os.path.join(path, base, name)
        if os.path.exists(filepath):
            f = codecs.open(filepath, 'r', encoding='utf-8')
            script = script + f.read()
            f.close()
            
    ext = os.path.splitext(s3_key)[1]
    
    import tempfile
    srcpath = tempfile.mktemp() + ext
    src = codecs.open(srcpath, 'w', encoding='utf-8')
    src.write(script)
    src.close()
    
    dstpath = tempfile.mktemp() + ext
    yuipath = os.path.join(path, 'utils/yuicompressor-2.4.7.jar')
    cmd = ['java', '-jar', yuipath, '-v', '-o', dstpath, srcpath]
    
    from subprocess import Popen, PIPE
    p = Popen(cmd, stdout=PIPE, stderr=PIPE)
    out, err = p.communicate()
    if p.returncode is 0:
        if ext == '.css':
            constent_type = 'text/css'
        elif ext == '.js':
            constent_type = 'text/javascript'
        else:
            constent_type = 'text/plain'
        print 'Script: ' + s3_key
        k.key = s3_key
        k.set_contents_from_filename(dstpath, headers={'Content-Type': constent_type}, policy='public-read')
        k.close()
        os.remove(dstpath)
    else:
        print '[[ error begin --------------------------------\nFailed:'
        print err
        print 'error end ---------------------------------- ]]'
    os.remove(srcpath)


# function to copy
def copy_image(root, rootkey):
    for base, dirs, files in os.walk(root, topdown=True):
        for name in files:
            ext = os.path.splitext(name)[1]
            if ext == '.png':
                content_type = 'image/png'
            else:
                content_type = None
            if content_type:
                path = os.path.join(base, name)
                key = os.path.join(rootkey, name)
                print 'Image: ' + path + ' → ' + key
                k.key = key
                k.set_contents_from_filename(path, headers={'Content-Type': content_type}, policy='public-read')
                k.close()
        for name in dirs:
            copy_image(os.path.join(base, name), os.path.join(rootkey, name))





from datetime import datetime
now = datetime.now().strftime('%Y%m%d%H%M%S')
targets = [
    {
        'base': 'ntools_media/static/css',
        'files': ['common.css'],
        'key': 'ntools_media/css/common-' + now + '.css'
    },
    {
        'base': 'ntools_media/static/js',
        'files': ['project.js'],
        'key': 'ntools_media/js/project-' + now + '.js'
    },
    {
        'base': 'ntools_media/static/js',
        'files': [
            'edit-utils.js',
            'edit-workbench.js',
            'edit-command.js',
            'edit-ui-resource.js',
            'edit-ui-audio.js',
            'edit-ui-graphic.js',
            'edit-ui-interact.js',
            'edit-ui-select.js',
            'edit-ui.js',
            'edit-ui-jquery.js',
            'edit-canvas.js',
            'edit-shortcut.js',
            'edit.js'],
        'key': 'ntools_media/js/edit-' + now + '.js'
    },
    {
        'base': 'ntools_media/static/js',
        'files': ['script.js'],
        'key': 'ntools_media/js/script-' + now + '.js'
    },
    {
        'base': 'ntools_media/static/js',
        'files': ['edit-s3-iframe.js'],
        'key': 'ntools_media/js/edit-s3-iframe.js'
    }
]

# compress, encode and upload to S3 js/css files
list = {'debug': {'js': [], 'css': []}, 'release': {'js': [], 'css': []}}
for t in targets:
    make_file(t['base'], t['files'], t['key'])
    for name in t['files']:
        ext = os.path.splitext(name)[1][1:]
        list['debug'][ext].append(name)
    name = os.path.split(t['key'])[1]
    ext = os.path.splitext(name)[1][1:]
    list['release'][ext].append(name)

# make static file list to read on release server
from django.utils import simplejson
static_path = os.path.join(path, 'static_files.json')
try:
    f = open(static_path, 'r')
    json = simplejson.loads(f.read())
    f.close()
except IOError:
    json = []
json.append(list)

f = open(static_path, 'w')
f.write(simplejson.dumps(json, indent=4))
f.close()



# copy images to s3
basekey = 'ntools_media/img'
basepath = os.path.join(path, 'ntools_media/static/img')
copy_image(basepath, basekey)
