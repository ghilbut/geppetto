# -*- coding: utf-8 -*-
import os, sys
from django.utils import simplejson



path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../'))
if path not in sys.path:
    sys.path.append(path)

# Djnago - ntools
os.environ['DJANGO_SETTINGS_MODULE'] = 'ntools.settings'



if False:
    from django.utils import simplejson
    from ntools.apps.account.models import Account
    from ntools.apps.project.models import Project
    names = [
        'epasskorea_1_1',
        'epasskorea_1_2',
        '1_3_epasskorea',
        '1_4_epasskorea',
        '1_5_epasskorea',
        '1_6_epasskorea',
        '1_7_epasskorea',
        '1_8_epasskorea',
        '1_9_epasskorea'
    ]
    for name in names:
        p = Project.objects.get(owner__username='admin', name=name, date_deleted=None)
        props = simplejson.loads(p.props)
        props['portrait'] = True
        props['landscape'] = True
        p.props = simplejson.dumps(props)
        p.save()
