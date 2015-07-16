#!/usr/bin/env python

import os
import sys
import optparse
import commands

EOSPREFIX='root://eoscms//eos/cms'

"""
processes locally or submits to batch the RECO step for a ROOT file or a directory of ROOT files
"""
def main():

    # configure
    usage = 'usage: %prog [options]'
    parser = optparse.OptionParser(usage)
    parser.add_option('-o', '--output',     dest='output' , 
                      help='Output directory (local or eos) [default=%default]', 
                      default='/store/cmst3/group/hgcal/TimingTB_H2_Jul2015/RECO/v0/')
    parser.add_option('-i', '--input',      dest='input',   
                      help='Input directory or file (local or eos) [default=%default]',
                      default='/store/group/dpg_ecal/alca_ecalcalib/TimingTB_H2_Jul2015/raw/DataTree/3351')
    parser.add_option('--base',      dest='base',
                      help='Directory with RECO software installation [default=%default]',
                      default=os.path.dirname(os.path.dirname(os.path.realpath(sys.argv[0]))))
    parser.add_option('-q', '--queue',      dest='queue', 
                      help='Batch queue [default=%default]',
                      default='local')
    (opt, args) = parser.parse_args()

    #build list of input files
    fileList=[]
    if opt.input.endswith('.root'):
        fileList.append(opt.input)
    else:
        listCmd='ls %s | grep -ir .root' % opt.input
        if '/store/' in opt.input:
            listCmd='cmsLs %s | awk \'{print $5}\'' % opt.input
        fileList=commands.getstatusoutput(listCmd)[1].split()

    #build list of tasks
    tasks=[]
    for f in fileList :
        if '/store/' in f and not EOSPREFIX in f:  f='%s/%s' % (EOSPREFIX,f)
        localOutputPrefix=''
        baseInputDir=os.path.dirname(f)
        localOutputPrefix='' if len(baseInputDir)==0 else os.path.basename( os.path.dirname(f) )+'_'
        localOutput='RECO_%s%s' % ( localOutputPrefix,os.path.basename(f) )
        tasks.append( (f,localOutput) )

    #prepare output
    prepareCmd = 'cmsMkdir' if '/store/' in opt.output else 'mkdir -p'
    commands.getstatusoutput('%s %s' % (prepareCmd,opt.output) )
    stageCmd   = 'cmsStage' if '/store/' in opt.output else 'cp'
        
    #run or submit tasks
    for inputUrl, localOutput in tasks:
        if opt.queue=='local':
            os.system('cd %s && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s/lib && ./bin/RunH4treeReco %s %s && %s %s %s && rm %s && cd -' 
                      % (opt.base,
                         opt.base,
                         inputUrl,localOutput,
                         stageCmd,localOutput,opt.output,
                         localOutput)
                      )
        else:
            scriptRealPath=os.path.realpath(sys.argv[0])
            os.system('bsub -q %s %s -o %s -i %s --base %s -q local'%(opt.queue,scriptRealPath,opt.output,inputUrl,opt.base))


if __name__ == '__main__':
    main()
