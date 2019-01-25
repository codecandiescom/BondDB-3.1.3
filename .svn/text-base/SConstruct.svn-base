#///BONDDB///
env = Environment()
# Hacked from http://www.scons.org/cgi-sys/cgiwrap/scons/moin.cgi/InstallTargets
import SCons
import os
from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.Chmod = SCons.Action.ActionFactory(os.chmod,
        lambda dest, mode: 'Chmod("%s", 0%o)' % (dest, mode))

def InstallPerm(env, dest, files, perm):
    obj = env.Install(dest, files)
    for i in obj:
        env.AddPostAction(i, env.Chmod(str(i), perm))
    return dest

def get_gid(groupname):
	passwd = open('/etc/group')
	for line in passwd:
		line = line.split(':')
		if line[0] == groupname:
			return int(line[2])
					 	
	print 'Could not find user ' + groupname + ' in /etc/group'
	return -1

SConsEnvironment.InstallPerm = InstallPerm

try:
    umask = os.umask(022)
except OSError:     # ignore on systems that don't support umask
    pass



opts = Options()
opts.Add(BoolOption('RELEASE', 'do a release build.', 'no'))
opts.Add(BoolOption('PROFILE', 'build with profiling turned on.', 'no'))
opts.Add(BoolOption('DOCS', 'Build docs using doxygen', 'no'))
opts.Add(BoolOption('XMINGW', 'Cross compile to win32', 'no'))
opts.Add(BoolOption('gda', 'gda database support', 'no'))
opts.Add(BoolOption('pg', 'postgresql database support', 'yes'))
opts.Add(BoolOption('mysql', 'mysql database support', 'no'))
opts.Add(BoolOption('oldlib', 'Compile the older bonddb version 1 library', 'no'))
opts.Add(BoolOption('OPT', 'install to opt instead of /usr/local', 'no'))

opts.Update(env)

def CheckPKG(context, name, version): 
	print "\n"
	context.Message( 'Checking for %s >= %s... ' % (name, version) )
	action='pkg-config %s --atleast-version=%s' % (name,version)
	print action
	ret = context.TryAction(action)
	context.Result( ret[0] )  
	return ret[0]

# Checks for libraries, header files, etc. go here!

#if not conf.CheckCHeader('string.h'):
#	print 'string.h must be installed.'
#	Exit(1)
#if not conf.CheckFunc('strcasecmp'):
#	print 'strcasecmp() can not be found.'
#	Exit(1)
# not available in windows - don't use !!!

conf = Configure(env, custom_tests = { 'CheckPKG' : CheckPKG })
fail = 0

if not env.GetOption('clean'):
	if not conf.CheckPKG('glib-2.0','2.12.0'):
		print 'Missing libglib >= 2.12.0'
		fail = 1

	if fail == 1 :
		print 'Missing dependencies; cannot build'
		Exit(1)

if env['RELEASE']==1 or env['PROFILE']==1:
	env.Append(CPPDEFINES = 'RELEASE_BUILD=1' )

# if env.Prepend(CPPATH=['bondsql']);

if env['XMINGW'] and env['PLATFORM'] != 'win32': 
	usr='/opt/cross-tools/i586-mingw32msvc/'
	usrlocal=usr+'local/'
	env.ParseConfig('pkg-config --cflags --libs glib-2.0 gmodule-2.0') 
	env.Tool('crossmingw', toolpath = ['SCons/Tool'])
	env.Append(CPPPATH=['/opt/cross-tools/i586-mingw32msvc/include'])

	if BUILD_TARGETS.count('install'):
		# try to install as group windeveloper...
		umask = os.umask(002)
		windeveloper_gid=get_gid('windeveloper');
		if windeveloper_gid>0:
			try:
			    	gid=os.getegid
				if gid != windeveloper_gid:
					os.setegid(windeveloper_gid)
			except	OSError:
				groupname=os.popen('id -ng').read()
				if cmp(groupname,'windeveloper\n'):
				   	print 'Got group=',groupname 
					print 'You must install as group windeveloper'
					print 'issue the command "newgroup windeveloper"'
	        			print 'then run "cross-scons install" or do'
					print '"sg windeveloper \'cross-scons install\'"'
					Exit(1)
else:		
   	env.ParseConfig('pkg-config --cflags --libs glib-2.0 gmodule-2.0') 
	if env['OPT']:
		usr='/usr/'
		usrlocal='/opt/bond/'
	else:
		usr='/usr/'
		usrlocal='/usr/local/'



pg='0'
if env['pg']:
   	pg='1'
	print 'Using postgresql backend.'
	if not conf.CheckLib('pq'):
		print 'libpq was not found.'
		Exit(1)
oldlib='0'
if env['oldlib']:
   	oldlib='1'
	print 'Compiling obsolete bonddb.so library.'


mysql='0'
if env['mysql']:
   	mysql='1'
	print 'Using mysql backend. - Obsolete'

gda='0'
if env['gda']:
   	gda='1'
	print 'Using gda backend. - Obsolete'
env = conf.Finish()

# add support for GLib etc

if env['RELEASE']==1 or env['PROFILE']==1:
	# Optomised version for speed
  	# env.Prepend(CCFLAGS = ['-O3','-mtune=i686'])
  	env.Prepend(CCFLAGS = ['-O3','-g'])
	print 'Building optimized release.'
else:
	# debuging  version
	env.Prepend(CCFLAGS = ['-g','-Wall','-DMEM_TEST'])
        # removed -ansi because it clashes with use of "fileno()"

if env['PROFILE']==1:
	env.Prepend(CCFLAGS = '-pg')
	print 'Building with profiling support.'
	
if env['XMINGW'] or env['PLATFORM'] == 'win32':
	env.Prepend(CCFLAGS = '-DWIN32');
else:
	env.Prepend(CCFLAGS = '-DUNIX');



Export('env','pg','mysql','gda','usr','usrlocal')
SConscript(['bondcommon/SConstruct'])

if int(pg):
	SConscript(['pgsql/SConstruct'])

if int(mysql):
	SConscript(['mysql/SConstruct'])

if int(gda):
	SConscript(['gda/SConstruct'])

SConscript(['bondsql/SConstruct',
	'bonddb/SConstruct'
])

if env['XMINGW']: 
	SConscript(['vb/SConstruct'])
   
#if int(oldlib):
#	SConscript(['src/SConstruct'])

Help(opts.GenerateHelpText(env))

if os.environ.has_key('PKG_CONFIG_PATH'):
	print "Using customised pkg-config path"
	print os.environ['PKG_CONFIG_PATH']
	pkgdir = os.environ['PKG_CONFIG_PATH']
elif env['XMINGW'] == 'win32': 
	pkgdir = usr+'lib/pkgconfig'
else:
	pkgdir = '/usr/lib/pkgconfig'
	

#env.InstallPerm(pkgdir,'bonddb.pc',0444)
env.InstallPerm(pkgdir,'bonddb2.pc',0444)
env.InstallPerm(pkgdir,'bondcommon.pc',0444)
env.Alias('install', pkgdir)

if env['DOCS']:
	nv = Environment(tools = ["default", "doxygen"], toolpath = '.')
	nv.Doxygen("Doxyfile")
# doxygen.py available at http://www.scons.org/wiki/DoxygenBuilder?action=AttachFile&do=get&target=doxygen.py
# or see http://bugzilla.treshna.com/show_bug.cgi?id=1228
