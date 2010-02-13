#!/usr/bin/env python

"""
theora_encoder.py

Front-end to various programs needed to create Ogg Theora
movies with some image-enhancing capabilities through the use of
ImageMagick. Meant as a companion to LiVES (to possibly call
from within a plugin, see http://www.xs4all.nl/~salsaman/lives/ )
but can also be used as a stand-alone program.

Requires MPlayer, ImageMagick (GraphicsMagick might also
work), sox, libtheora, and Python 2.3.0 or greater.

Copyright (C) 2004-2005 Marco De la Cruz (marco@reimeika.ca)
It's a trivial program, but might as well GPL it, so see:
http://www.gnu.org/copyleft/gpl.html for license.

See my vids at http://amv.reimeika.ca !
"""

version = '0.1.6'
convert = 'convert'
mplayer = 'mplayer'
theora_encoder = 'encoder_example'
theora_encoder2 = 'theora_encoder_example'
    
sox = 'sox'
identify = 'identify'

usage = \
      """
theora_encoder.py -h
theora_encoder.py -V
theora_encoder.py -C
theora_encoder.py [-o out] [-p pre] [-d dir] [-a aspect] [-D delay*]
                  [-q|-v] [-t type] [-k] [-e [[-w dir] [-c geom] [-r geom]]]
                  [-s sndfile] [-b sndrate] [-f fpscode] [-L lv1file]
                  [firstframe lastframe]
      """

help = \
     """
SUMMARY (ver. %s):

Encodes a series of PNG or JPG images into an Ogg Theora
(.ogg) stream and is also capable of performing some simple image
enhacements using ImageMagick. The images and audio are assumed to
be in an uncompressed LiVES format, hence making this encoder
more suitable to be used within a LiVES plugin (but can also
be used directly on a LiVES temporary directory).

OPTIONS:

-h          for help.

-V          shows the version number.

-C          check external program dependencies and exit.

-o out      will save the video in file "out".
            Default: "'type'_movie.ogg". See below for 'type'.

-p pre      the encoder assumes that sound and images are named using
            LiVES' conventions i.e. "audio" and "00000001.ext",
            "00000002.ext"... where "ext" is either "jpg" or "png".
            However, theora_encoder.py will create temporary files
            which can be saved for later use (for example, after
            enhancing the images with "-e" , which may take a long
            time). These temporary files will be named
            "pre_00000001.ext", etc... where "pre" is either "eimg"
            or "rimg" and will be kept if the "-k" option is used.
            theora_encoder.py can then be run over the enhanced images
            only by selecting the appropriate prefix ("eimg" or
            "rimg") here. See "-e" to see what each prefix means.
            Default: "" (an empty string).

-d dir      "dir" is the directory containing the source image
            files. These must be of the form "00000001.ext" or
            "pre_00000001.ext" as explained above.
            Default: "."

-a aspect   sets the aspect ratio of the resulting Theora file. This can be:

               1    1:1 display
               2    4:3 display
               3    16:9 display
               4    2.21:1 display

            Default: "2"

-D delay*   linearly delay the audio stream by "delay" ms.
            Default: "0"
            * = NOT IMPLEMENTED (YET?)

-q          quiet operation, only complains on error.

-v          be verbose.

-t type     type of video created. The options here are:

               "hi":   a very high quality movie, suitable
                       for archiving images of 720x480.

               "mh":   medium-high quality movie, which still allows
                       to watch small videos (352x240 and below)
                       fullscreen.

               "ml":   medium-low quality movie, suitable for
                       most videos but in particular small videos
                       (352x240 and below) meant for online
                       distribution.

               "lo":   a low quality movie format. Enlarging
                       small videos will result in noticeable
                       artifacts. Good for distributing over slow
                       connections.

            Default: "ml"

-e          perform some simple filtering/enhancement to improve image
            quality. The images created by using this option will be
            stored in the directory specified by the "-w" option (or
            "-d" if not present, see above) and each filename will be
            prefixed with "eimg" (see "-p" above). Using this option will
            take enormous amounts of disk space, and in reality does
            not do anything that cannot be done within LiVES itself.
            Using this option enables the following four:

            -w dir      "dir" is the working directory where the enhanced
                        images  will be saved. Although these can all
                        co-exist with the original LiVES files it is
                        recommended you use a different directory. Note
                        that temporary files may take a huge amount of
                        space, perhaps twice as much as the LiVES files
                        themselves. If this directory does not exist is
                        will be created.
                        Default: same as "-d".

            -k         keep the temporary files, useful to experiment
                       with different encoding types without having
                       to repeatedly enhance the images (which can be
                       very time-consuming). See "-p" above.

            -c geom    in addition to enhancing the images, crop them
                       to the specified geometry e.g. "688x448+17+11".
                       Filename prefix remains "eimg". Note that the
                       dimensions of the images should both be multiples
                       of "16", otherwise some players may show artifacts
                       (such as garbage or green bars at the borders).

            -r geom    this will create a second set of images resulting
                       from resizing the images that have already been
                       enhanced and possibly cropped (if "-c" has been
                       specified). The geometry here is simple a new
                       image size e.g. "352x240!". This second set will have
                       filenames prefixed with the string "rimg" (see "-p"
                       above). Note that the dimensions of the images
                       should both be multiples of "16", otherwise some
                       players may show artifacts (such as garbage or
                       green bars at the borders).

-s sndfile  name of the audio file. Can be either raw "audio" or
            "[/path/to/]soundfile.wav".
            Default: "audio" inside "dir" as specified by "-d".

-b sndrate  sample rate of the sound file in Hertz.
            Default: "44100".

-f fpscode  frame-rate code. Acceptable values are:

              1 - 24000.0/1001.0 (NTSC 3:2 pulldown converted FILM)
              2 - 24.0 (NATIVE FILM)
              3 - 25.0 (PAL/SECAM VIDEO / converted FILM)
              4 - 30000.0/1001.0 (NTSC VIDEO)
              5 - 30.0
              6 - 50.0 (PAL FIELD RATE)
              7 - 60000.0/1001.0 (NTSC FIELD RATE)
              8 - 60.0

            If the input value is a float (e.g. 5.0, 14.555, etc.)
            the encoder will use that as frame rate.

            Default: "4".

-L lv1file  use the images stored in the LiVES file "lv1file" (e.g.
            "movie.lv1"). Files will be extracted inside "dir" as
            specified by "-d". Using this option allows to encode
            a movie without having to restore it first with LiVES.
            Note that the encoder will not delete the extracted
            files after it has finished, this must be done manually
            afterwards. If frame code/rate (-f) and/or sound rate (-b)
            are not specified the defaults will be used (30000/1001 fps
            and 44100 Hz). These, however, may not be correct, in which
            case they should be specified explicitly. Furthermore, the
            .lv1 audio (if present) is always assumed to have a sample
            data size in 16-bit words with two channels, and to be signed
            linear (this is usually the case, however). If any of these
            latter parameters are incorrect the resulting file may be
            corrupted and encoding will have to be done using LiVES.

firstframe  first frame number. If less than eight digits long it will
            be padded with zeroes e.g. 234 -> 00000234. A prefix can
            be added using the "-p" option.
            Default: "00000001"

lastframe   last frame number, padded with zeroes if necessary, prefix
            set by "-p".
            Default: the last frame of its type in the "-d" directory,
                     where "type" is set by the prefix.

Note that either no frames or both first and last frames must be specified.

EXAMPLES:

Suppose you have restored a LiVES' .lv1 file (in either JPG or PNG format),
and that the images are stored in the directory "/tmp/livestmp/991319584/".
In this example the movie is assumed to be 16:9. Then, in order to create
an ogg file you can simply do the following:

   theora_encoder.py -d /tmp/livestmp/991319584 -o /movies/default.ogg -a 3

and the clip "default.ogg" will be created in "/movies/" with the correct
aspect ratio.

Suppose we want to make a downloadable version of the clip, small in size
but of good quality. The following command activates the generic enhancement
filter and resizes the images to "352x240". Note that these operations
are performed after cropping the originals a bit (using "704x464+5+6").
This is done because we are assuming that there is a black border around
the original pictures which we want to get rid of (this might distort the
images somewhat, so be careful about cropping/resizing, and remember to
use dimensions which are multiples of 16):

   theora_encoder.py -v -d /tmp/livestmp/991319584 -w /tmp/tmpogg \\
   -o /movies/download.ogg -a 3 -k -e -c "704x464+5+6" -r "352x240"

Since we use the "-k" flag the enhanced images are kept (both full-size
crops and the resized ones) in "/tmp/tmpogg". Beware that this may consume
a lot of disk space (about 10x as much as the originals). The reason we
keep them is because the above may take quite a long time and we may want
to re-use the enhanced images. So, for example, creating a high-quality
clip at full size can be accomplished now as follows:

   theora_encoder.py -d /tmp/tmpogg -t hi -o /movies/archive.ogg \\
   -s /tmp/livestmp/991319584/audio -a 3 -k -p eimg

If, for example, we only want to encode frames 100 to 150 we can run
the following:

   theora_encoder.py -v -d /tmp/tmpogg -o /movies/selection.ogg \\
   -a 3 -k -p eimg 100 150

Note that no audio has been selected ("-s"). This is because
theora_encoder.py cannot trim the audio to the appropriate length.
You would need to use LiVES to do this.

To delete all the enhanced images you can just remove "/tmp/tmpogg".

If you notice that the video and audio become out of sync so that
near the end of the movie the video trails the audio by, say, 0.2s,
you can use the "-D" option as follows:

   theora_encoder.py -d /tmp/livestmp/991319584 -o test.ogg -D 200

Note that negative time values are also allowed ("-D" NOT IMPLEMENTED YET).

Suppose that you have "movie1.lv1", "movie2.lv1" and "movie3.lv1".
Batch-encoding can be done as follows (zsh-syntax):

   for i in movie?.lv1
   do
     mkdir /tmp/$i:r
     theora_encoder.py -d /tmp/$i:r -o /tmp/$i:r.ogg -L $i
     rm -rf /tmp/$i:r
   done

This will generate the files "movie1.ogg", "movie2.ogg" and
"movie3.ogg" in "/tmp". Note that is is not necessary to
specify whether the files are stored in JPG or PNG format,
and that potentially time-consuming options (e.g. "-e") may
be enabled. It is not necessary to have a working LiVES
installation to encode .lv1 files.
     """ % version


def gcd(a, b):
    """
    gcd(a, b) (a,b are integers)
    Returns the greatest common divisor of two integers.
    """
    if b == 0: return a
    else: return gcd(b, a%b)
    

def frame2pixel_aspect(size, frame_aspect):
    pixel_den = int(frame_aspect[1])*int(size[0])
    pixel_num = int(frame_aspect[0])*int(size[1])

    divisor = gcd(pixel_num, pixel_den)
    if divisor > 1:
	    pixel_num = pixel_num/divisor
	    pixel_den = pixel_den/divisor
    pixel_aspect = (pixel_num, pixel_den)
    return pixel_aspect


def run(command):
    """
    Run a shell command
    """

    if verbose:
        print('Running: \n' + command + '\n=== ... ===')
        std = ''
    else:
        std = ' > /dev/null 2>&1'

    os.system(command + std)


def do_enhance():
    """
    Image cleanup/crop/resize routine. Generic, but seems to work
    well with anime :)
    """

    if not quiet:
        print('Enhancing images... please wait, this might take long...')

    enh_opts = "-enhance -sharpen '0.0x0.5' -gamma 1.2 -contrast -depth 8"

    if cgeom:
        docrop = ' '.join(['-crop', "'%s!'" % cgeom])
    else:
        docrop = ''

    iframe = first_num
    while True:
        # File names are padded with zeroes to 8 characters
        # (not counting the .ext).
        frame = str(iframe).zfill(8)
        fname = os.path.join(img_dir, frame + ext)
        if not os.path.isfile(fname) or (iframe == last_num + 1):
            break
        eimg_fname = os.path.join(work_dir, 'eimg' + frame + '.png')
        rimg_fname = os.path.join(work_dir, 'rimg' + frame + '.png')
        command = ' '.join([convert, docrop, enh_opts, fname, eimg_fname])
        run(command)
        if rgeom:
            if os.path.exists(rimg_fname): os.remove(rimg_fname)
            shrink = ' '.join([convert, '-resize', "'%s!'" % rgeom, '-depth 8'])
            command = ' '.join([shrink, eimg_fname, rimg_fname])
            run(command)
        else:
            if os.path.exists(rimg_fname): os.remove(rimg_fname)
	    try:
            	os.symlink(eimg_fname, rimg_fname)
	    except (IOError, OSError):
		shutil.copy(eimg_fname, rimg_fname)
        iframe+=1


def do_encode():
    """
    Encode a series of images into a Theora movie.
    """

    if verbose:
        std = ''
        std2 = ''
        be_verbose = '-verbose'
    else:
        std = ' > /dev/null 2>&1'
        std2 = ' 2> /dev/null'
        be_verbose = ''

    if which(theora_encoder2) != '':
        theora_encoder = theora_encoder2

    fpsnum = fps.split('/')[0]
    fpsden = fps.split('/')[1]
    mplayer_opts = '-mf type=%s:fps=%s ' % (ext[1:],
                                            eval('float('+fpsnum+')/'+fpsden)) + \
                   '-vf hqdn3d=2:1:3,pp=va:128:8/ha:128:8/dr,dsize=%s -vo yuv4mpeg -ao null -nosound' % aspect

    if img_dir != work_dir and not enhance:
        source_dir = img_dir
    else:
        source_dir = work_dir

    fgeom = os.popen(' '.join([identify, '-format "%w %h"', \
                               os.path.join(source_dir, \
                                            img_pre + \
                                            first_frame_num + \
                                            ext)])).read().strip().split()

    par = frame2pixel_aspect(fgeom, aspect.split(':'))
    theora_opts = '-s %s -S %s -f %s -F %s' % (par[0], par[1], fpsnum, fpsden)
    if vtype == 'hi':
        theora_opts = ' '.join([theora_opts, '-v 10 -a 10'])
    elif vtype == 'mh':
        theora_opts = ' '.join([theora_opts, '-v 8 -a 6'])
    elif vtype == 'ml':
        theora_opts = ' '.join([theora_opts, '-v 6 -a 4'])
    elif vtype == 'lo':
        theora_opts = ' '.join([theora_opts, '-v 4 -a 2'])

    if frame_range:
        numframes = str(last_num - first_num + 1)
        frame_list = [img_pre + str(f).zfill(8) + ext \
                      for f in range(first_num, last_num + 1)]
        syml = 'temporary_symlink_'
        for iframe in frame_list:
            frfile = os.path.join(source_dir, iframe)
            frlink = os.path.join(source_dir, syml + iframe)
            if os.path.islink(frlink): os.remove(frlink)
            os.symlink(frfile, frlink)
    else:
        syml = ''
        numframes = len(glob.glob(os.path.join(source_dir, \
                                             img_pre + '*' + ext)))

    if audio:
        if rawsndf:
            wav = tempfile.mkstemp('.wav', '', work_dir)[1]
            sox_opts = '-t raw -r %s -w -c 2 -s' % sndr
            command = ' '.join([sox, sox_opts, sndf, wav])
            run(command)
        else:
            wav = sndf
    else:
        wav = ''

    all_vars = {}
    all_vars.update(globals())
    all_vars.update(locals())

    if not quiet:
        print('Creating "%s"-quality Theora file' % vtype)

    # mplayer with "-vo yuv4mpeg" always outputs to "stream.yuv"
    # maybe create the pipe pythonically?
    command = """cd %(source_dir)s ; \\
mkfifo -m 0600 stream.yuv ; \\
%(mplayer)s mf://%(syml)s%(img_pre)s*%(ext)s %(mplayer_opts)s %(std)s &
""" % all_vars
    run(command)
    command = """cd %(source_dir)s ; \\
%(theora_encoder)s %(theora_opts)s -o "%(vidname)s" %(wav)s stream.yuv %(std)s
""" % all_vars
    run(command)

    if os.path.exists(os.path.join(source_dir, 'stream.yuv')):
        os.remove(os.path.join(source_dir, 'stream.yuv'))

    if rawsndf and os.path.exists(wav): os.remove(wav)

    if frame_range:
        lframes = os.path.join(source_dir, syml)
        for frame in glob.glob(lframes + '*'):
            os.remove(frame)


def do_clean():
    """
    Delete enhanced files
    """

    if not quiet:
        print('Deleting all enhanced images (if any)')

    eframes = os.path.join(work_dir, 'eimg')
    rframes = os.path.join(work_dir, 'rimg')
    for frame in glob.glob(eframes + '*'):
         os.remove(frame)
    for frame in glob.glob(rframes + '*'):
         os.remove(frame)


def which(command):
    """
    Finds (or not) a command a la "which"
    """

    command_found = False

    if command[0] == '/':
        if os.path.isfile(command) and \
               os.access(command, os.X_OK):
            command_found = True
            abs_command = command
    else:
        path = os.environ.get('PATH', '').split(os.pathsep)
        for dir in path:
            abs_command = os.path.join(dir, command)
            if os.path.isfile(abs_command) and \
               os.access(abs_command, os.X_OK):
                command_found = True
                break

    if not command_found:
        abs_command = ''

    return abs_command


def is_installed(prog):
    """
    See whether "prog" is installed
    """

    wprog = which(prog)

    if wprog == '':
        if prog == theora_encoder:
            if which(theora_encoder2) == '':
                print(theora_encoder + ' or ' + theora_encoder2 + ': command not found')
                raise SystemExit(1)
        else:
            if prog != theora_encoder2:
                print(prog + ': command not found')
                raise SystemExit(1)
        
    else:
        if verbose:
            print(wprog + ': found')


if __name__ == '__main__':

    import os
    import sys
    import getopt
    import shutil
    import tempfile
    import glob
    import tarfile

    try:
        if sys.version_info[0:3] < (3, 0, 0):
            raise SystemExit(1)
    except:
        print('You need Python 3.0.0 or greater to run me!')
        raise SystemExit(1)

    try:
        (opts, args) = getopt.getopt(sys.argv[1:], \
                                         'ho:p:d:w:a:qvt:ekc:r:s:b:f:VCD:L:')
    except:
        print("Something's wrong. Try the '-h' flag.")
        raise SystemExit(1)

    opts = dict(opts)

    if not opts and not args:
        print(usage)
        raise SystemExit(1)

    if '-h' in opts:
        print(usage + help)
        raise SystemExit

    if '-V' in opts:
        print('theora_encoder.py version ' + version)
        raise SystemExit

    if ('-v' in opts) or ('-C' in opts):
        verbose = True
    else:
        verbose = False

    if '-q' in opts:
        quiet = True
        verbose = False
    else:
        quiet = False

    for i in [mplayer, theora_encoder, theora_encoder2, sox, convert, identify]:
        is_installed(i)
    if '-C' in opts: raise SystemExit

    img_pre = opts.get('-p', '')

    if img_pre not in ['', 'eimg', 'rimg']:
         print('Improper image name prefix.')
         raise SystemExit(1)

    temp_dir = ''
    img_dir = opts.get('-d', '.')
    img_dir = os.path.abspath(img_dir)
    if ' ' in img_dir:
        temp_dir = tempfile.mkdtemp('', '.lives-', '/tmp/')
        os.symlink(img_dir, temp_dir + '/img_dir')
        img_dir = temp_dir + '/img_dir'

    if not os.path.isdir(img_dir):
        print('The image source directory: '  + img_dir + \
              ' does not exist!')
        raise SystemExit(1)

    if len(args) not in [0, 2]:
        print('If frames are specified both first and last ' + \
              'image numbers must be chosen.')
        raise SystemExit(1)
    elif len(args) == 0:
        args = [None, None]

    frame_range = False
    if not args[0]:
        first_frame_num = '1'
    else:
        frame_range = True
        first_frame_num = args[0]
    first_frame_num = first_frame_num.zfill(8)
    first_num = int(first_frame_num)

    if not args[1]:
        last_frame_num = '99999999'
    else:
        last_frame_num = args[1]
    last_frame_num = last_frame_num.zfill(8)
    last_num = int(last_frame_num)

    aspectc = opts.get('-a', '2')

    if aspectc not in [str(i) for i in range(1,5)]:
        print('Invalid aspect ratio.')
        raise SystemExit(1)
    else:
        if aspectc == '1': aspect = '1:1'
        elif aspectc == '2': aspect = '4:3'
        elif aspectc == '3': aspect = '16:9'
        elif aspectc == '4': aspect = '221:100'

    vtype = opts.get('-t', 'ml')

    if vtype not in ['hi', 'mh', 'ml', 'lo']:
        print('Invalid video type.')
        raise SystemExit(1)

    out_ogg = opts.get('-o', vtype + '_movie.ogg')

    fpsc = opts.get('-f', '4')

    if fpsc not in [str(i) for i in range(1,9)]:
        if not quiet: print('Invalid fps code, attempting float fps.')
        foundfps = False
    else:
        if fpsc == '1': fps = '24000/1001'
        elif fpsc == '2': fps = '24/1'
        elif fpsc == '3': fps = '25/1'
        elif fpsc == '4': fps = '30000/1001'
        elif fpsc == '5': fps = '30/1'
        elif fpsc == '6': fps = '50/1'
        elif fpsc == '7': fps = '60000/1001'
        elif fpsc == '8': fps = '60/1'
        foundfps = True

    if not foundfps:
        try:
            fpsnum = int(locale.atof(fpsc)*10**15)
            fpsden = 10**15
            divisor = gcd(fpsnum, fpsden)
            if divisor > 1:
                fpsnum = fpsnum/divisor
                fpsden = fpsden/divisor
            fps = '%s/%s' % (fpsnum, fpsden)
            if not quiet: print('Using fps = ' + fps)
            if fpsnum > 0: foundfps = True
        except:
            pass

    if not foundfps:
        print('Invalid fps code or rate.')
        raise SystemExit(1)

    if '-e' not in opts:
        enhance = False
    else:
        enhance = True

    if enhance and img_pre:
        print('Sorry, you cannot enhance already-enhanced images')
        raise SystemExit(1)

    if '-k' not in opts:
        keep = False
    else:
        keep = True

    cgeom = opts.get('-c', '')
    rgeom = opts.get('-r', '')

    if (cgeom or rgeom) and not enhance:
        print('Missing "-e" option.')
        raise SystemExit(1)

    delay = opts.get('-D', '0')
    if verbose: print('Linear audio delay (ms): ' + delay)

    lv1file = opts.get('-L', None)
    if lv1file:
        if not quiet: print('Opening lv1 file...')
        try:
            lv1 = tarfile.open(os.path.abspath(lv1file))
        except:
            print('This does not appear to be a valid LiVES file!')
            raise SystemExit(1)
        if 'header.tar' not in lv1.getnames():
            print('This does not appear to be a valid LiVES file!')
            raise SystemExit(1)
        for tfile in lv1.getmembers():
            lv1.extract(tfile, img_dir)
        for tfile in glob.glob(os.path.join(img_dir, '*.tar')):
            imgtar = tarfile.open(tfile)
            for img in imgtar.getmembers():
                imgtar.extract(img, img_dir)
            os.remove(tfile)

    test_file = os.path.join(img_dir, img_pre + first_frame_num)
    if os.path.isfile(test_file + '.jpg'):
        ext = '.jpg'
    elif os.path.isfile(test_file + '.png'):
        ext = '.png'
    else:
        print('Cannot find any appropriate %s or %s files!' % ('.jpg','.png'))
        raise SystemExit(1)
    first_frame = test_file + ext
    geom = os.popen(identify + ' -format "%w %h" ' + first_frame).read().strip().split()
    last_frame = os.path.join(img_dir, img_pre + last_frame_num + ext)

    if not quiet: print('Found: ' + first_frame + ' (%sx%s)' % (geom[0], geom[1]))
    
    work_dir = opts.get('-w', img_dir)
    work_dir = os.path.abspath(work_dir)
    if not os.path.isdir(work_dir):
        if not quiet: print('Creating ' + work_dir)
        try:
            os.makedirs(work_dir)
            os.chmod(work_dir, 0o755)
        except:
            print('Could not create the work directory ' + \
                  work_dir)
            raise SystemExit(1)
    if ' ' in work_dir:
        if temp_dir == '':
            temp_dir = tempfile.mkdtemp('', '.lives-', '/tmp/')
        os.symlink(work_dir, temp_dir + '/work_dir')
        work_dir = temp_dir + '/work_dir'

    sndf = opts.get('-s', os.path.join(img_dir, 'audio'))
#    sndf = os.path.abspath(sndf)
    rawsndf = True
    if not os.path.isfile(sndf):
        audio = False
        rawsndf = False
    else:
        audio = True
        if sndf[-4:] == '.wav':
            rawsndf = False
        if not quiet: print('Found audio file: ' + sndf)

    sndr = opts.get('-b', '44100')

    if enhance:
        do_enhance()
        # Note that do_enhance() always creates images prefixed
        # with 'rimg'. What's important to note if that if the
        # images are resized the 'rimg' are indeed resized, but
        # if not the 'rimg' are simply symlinks to the 'eimg'
        # (enhanced) images.
        img_pre = 'rimg'
        ext = '.png'
    vidname = os.path.join(work_dir, out_ogg)
    # do_encode() acts on images prefixed by img_pre.
    do_encode()
    if not keep:
        do_clean()
    if temp_dir != '':
        shutil.rmtree(temp_dir)
    if not quiet: print("Done!")


"""
CHANGELOG:

25 Sep 2004 : 0.0.1 : first release.
01 Oct 2004 : 0.0.2 : made all fps fractions.
13 Oct 2004 : 0.0.3 : fix aspect ratio by using
                      pixel aspect ratio instead of
                      frame aspect ratio (j@v2v.cc).
		      calculate frame size to do the above.
		      fixed extension from ".ogv" to ".ogg".
14 Oct 2004 : 0.0.4 : "size" is (width, height) (original
                      patch had assumed it switched).
26 Oct 2004 : 0.0.5 : can handle arbitrary fps values.
02 Nov 2004 : 0.0.6 : tweaked encoding quality levels.
08 Nov 2004 : 0.0.7 : make sure that the enhanced
                      color depth is 8-bits/channel.
                      tweaked encoding quality levels.
02 Jan 2005 : 0.0.8 : updated docs.
                      added sound rate (-b) option.
22 Jan 2005 : 0.0.9 : fixed stream.yuv fps calculation.
11 Feb 2005 : 0.1.0 : fixed aspect ratio error (my bad :( )
26 Mar 2005 : 0.1.1 : use env python (hopefully >= 2.3).
                      replace denoise3d with hqdn3d.
24 Aug 2005 : 0.1.2 : fine-tune mplayer filters (thanks to
                      zikzak@tele2.fr).
                      Fixed 2.21:1 aspect ratio.
09 Mar 2006 : 0.1.3 : added '-depth 8' to resize, as ImageMagick
                      keeps changing its damn behaviour.
02 Apr 2006 : 0.1.4 : small aspect ratio bug fixed.
                      tweaked audio quality settings.
28 Jun 2007 : 0.1.5 : handles paths with spaces appropriately
                      (thanks Gabriel).
"""
