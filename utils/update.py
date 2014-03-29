# -*- coding: utf-8 -*-
import os, sys
from django.utils import simplejson



path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../'))
if path not in sys.path:
    sys.path.append(path)

# Djnago - ntools
os.environ['DJANGO_SETTINGS_MODULE'] = 'ntools.settings'



# 2012-08-08 Chnage data structure for javascript
# UI expression of graphics and interactions handled with absoleute number by each field named zindex and order.
# They will removed from table and view props will contain their relation.
# You can see them with array about related position of.
if True:
    from ntools.apps.account.models import Account
    from ntools.apps.project.models import Project
    ps = Project.objects.filter(date_deleted=None).order_by('date_updated')
    for p in ps:
        # project
        p_props = simplejson.loads(p.props)
        p_props['id'] = p.pid
        p_props['name'] = p.name
        
        print p.pid + ' : ' + p.name

        # page([s]heet) list
        pages = []
        ss = p.sheet_set.filter(date_deleted=None).order_by('order')
        for s in ss:
            pages.append(s.sid)
        p_props['pages'] = pages

        # view list
        vs = p.view_set.filter(date_deleted=None)
        for v in vs:
            v_props = simplejson.loads(v.props)
            v_props['id'] = v.vid
            v_props['type'] = v.type
            v_props['name'] = v.name
            v_props['preview'] = v.preview
            v_props['is_validate'] = True
            
            # a[u]dio list
            audios = []
            us = v.element_set.filter(type='music', date_deleted=None).order_by('name')
            for u in us:
                u_props = simplejson.loads(u.props)
                u_props['owner'] = v.vid
                u_props['id'] = u.eid
                u_props['type'] = u.type
                u_props['name'] = u.name
                u_props['is_validate'] = True
                u.props = simplejson.dumps(u_props)
                u.save()
                audios.append(u.eid)
            
            # [g]raphics list
            graphics = []
            gs = v.element_set.filter(zindex__gt=0, date_deleted=None).order_by('-zindex')
            for g in gs:
                g_props = simplejson.loads(g.props)
                g_props['owner'] = v.vid
                g_props['id'] = g.eid
                g_props['type'] = g.type
                g_props['name'] = g.name
                g_props['is_validate'] = True
                g.props = simplejson.dumps(g_props)
                g.save()
                graphics.append(g.eid)
    
            # i[n]teract list
            interacts = []
            ns = v.interact_set.filter(date_deleted=None).order_by('order')
            for n in ns:
                n_props = simplejson.loads(n.props)
                n_props['owner'] = v.vid
                n_props['id'] = n.iid
                n_props['source'] = n.source
                n_props['event'] = n.event
                n_props['target'] = n.target
                n_props['action'] = n.action
                n_props['is_validate'] = True
                n.props = simplejson.dumps(n_props)
                n.save()
                interacts.append(n.iid)

            v_props['audios'] = audios 
            v_props['graphics'] = graphics 
            v_props['interacts'] = interacts 
            v.props = simplejson.dumps(v_props)
            v.save()
        
        p.props = simplejson.dumps(p_props)
        p.save()
        
        '''
        ALTER TABLE `nt_elements` DROP `zindex`;
        ALTER TABLE `nt_interacts` DROP `order`;
        '''



# 2012-08-03 Change ntools.nt_resources : append props and remove thumb field
if False:
    '''
    ALTER TABLE  `nt_resources` ADD  `props` LONGTEXT CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL AFTER  `thumb`;
    '''
    
    import boto
    from boto.s3.key import Key
    from ntools.settings import AWS_PUBLIC_KEY, AWS_SECRET_KEY, AWS_NTOOLS_BUCKET
    s3 = boto.connect_s3(AWS_PUBLIC_KEY, AWS_SECRET_KEY)
    b = s3.get_bucket(AWS_NTOOLS_BUCKET)
    k = Key(b)
    
    from ntools.apps.account.models import Account
    from ntools.apps.project.models import Project
    from ntools.apps.resource.models import Resource
    rs = Resource.objects.select_related('project__pid', 'rid', 'name', 'props').filter(project__date_deleted__isnull=True, date_deleted=None)
    count = 0;
    for r in rs[count:]:
        pid = r.project.pid
        name = r.name
        k.key = os.path.join(pid, name)
        props = {
            'bucket': AWS_NTOOLS_BUCKET,
            'key': k.key
        }
        
        if r.type == 'audio':
            pass
        elif r.type == 'image':
            # get width and height
            import urllib2
            from cStringIO import StringIO
            from PIL import Image, ImageDraw
            url = os.path.join('http://', AWS_NTOOLS_BUCKET, pid, name)
            print url
            buf = StringIO(urllib2.urlopen(url).read()) 
            base = Image.open(buf)
            # if base.mode != 'RGBA':
            #    base = base.convert('RGBA')
            props.update({ 
                'width': base.size[0], 
                'height': base.size[1], 
                'thumb': r.thumb
            })
        elif r.type == 'video':
            pass
        else:
            pass
        
        r.props = simplejson.dumps(props)
        r.save()
        
        count = count + 1;
        print str(count) + ' / ' + str(len(rs)) + ' : ' + r.name;
    
    '''
    ALTER TABLE `nt_resources` DROP `thumb`;
    '''
