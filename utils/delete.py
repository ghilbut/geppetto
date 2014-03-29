# -*- coding: utf-8 -*-
import os, sys
from django.utils import simplejson



path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../'))
if path not in sys.path:
    sys.path.append(path)

# Djnago - ntools
os.environ['DJANGO_SETTINGS_MODULE'] = 'ntools.settings'



import boto
from boto.s3.key import Key
from ntools.settings import AWS_PUBLIC_KEY, AWS_SECRET_KEY, AWS_NTOOLS_BUCKET
s3 = boto.connect_s3(AWS_PUBLIC_KEY, AWS_SECRET_KEY)
b = s3.get_bucket(AWS_NTOOLS_BUCKET)
k = Key(b)

failed_count = 0

# project
from ntools.apps.account.models import Account
from ntools.apps.project.models import Project
ps = Project.objects.select_related().filter(date_deleted__isnull=False)
for p in ps:
    keys = b.get_all_keys(prefix=p.pid)
    for k in keys:
        if k.exists():
            k.delete()
            print 'Delete :: ' + k.key
        else:
            failed_count = failed_count + 1
            print 'Failed :: ' + k.key
    p.delete()
    print 'Delete :: ' + p.name + '(' + p.pid + ') :: ' + str(p.date_deleted)



# page
from ntools.apps.sheet.models import Sheet
ss = Sheet.objects.select_related('project__pid', 'sid', 'date_deleted').filter(date_deleted__isnull=False)
for s in ss:
    s.delete()
    print 'Delete :: ' + s.project.pid + ' :: ' + s.sid + ' :: ' + str(s.date_deleted)



# view
from ntools.apps.view.models import View
vs = View.objects.select_related('project__pid', 'vid', 'date_deleted').filter(date_deleted__isnull=False)
for v in vs:
    pid = v.project.pid
    k.key = os.path.join(pid, 'preview', v.vid + '.png')
    if k.exists():
        k.delete()
        print 'Delete :: ' + k.key
    else:
        failed_count = failed_count + 1
        print 'Failed :: ' + k.key
    v.delete()
    print 'Delete :: ' + pid + ' :: ' + v.vid + ' :: ' + str(v.date_deleted)



# element
from ntools.apps.element.models import Element
es = Element.objects.select_related('view__project__pid', 'eid', 'date_deleted').filter(date_deleted__isnull=False)
for e in es:
    e.delete()
    print 'Delete :: ' + e.view.project.pid + ' :: ' + e.eid + ' :: ' + str(e.date_deleted)



# i[n]teractions
from ntools.apps.interaction.models import Interact
ns = Interact.objects.select_related('view__project__pid', 'iid', 'date_deleted').filter(date_deleted__isnull=False)
for n in ns:
    n.delete()
    print 'Delete :: ' + n.view.project.pid + ' :: ' + n.iid + ' :: ' + str(n.date_deleted)



# resource
from ntools.apps.resource.models import Resource
rs = Resource.objects.select_related('project__pid', 'rid', 'name', 'date_deleted').filter(date_deleted__isnull=False)
for r in rs:
    pid = r.project.pid
    name = r.name
    k.key = os.path.join(pid, name)
    if k.exists():
        k.delete()
        print 'Delete :: ' + k.key
    else:
        failed_count = failed_count + 1
        print 'Failed :: ' + k.key
    
    if r.type == 'audio':
        pass
    elif r.type == 'image':
        k.key = os.path.join(pid, 'thumb', r.rid + '.png')
        if k.exists():
            k.delete()
            print 'Delete :: ' + k.key
        else:
            failed_count = failed_count + 1
            print 'Failed :: ' + k.key
    elif r.type == 'video':
        pass
    else:
        pass
    r.delete()
    print 'Delete :: ' + pid + ' :: ' + name + ' :: ' + str(r.date_deleted);

print 'Failed Count :: ' + str(failed_count)