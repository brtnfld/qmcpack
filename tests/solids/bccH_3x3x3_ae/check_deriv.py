#!/usr/bin/env python

def parse_deriv_block(mm,header,nmax_deriv=1024):
  """ parse overlap/hamiltonian matrix derivatives
   
   e.g.
  Deriv  Numeric Analytic
  0  -0.07248163314  -0.07248151057  -1.225712021e-07
  1  4.129043141  4.129043225  -8.402355167e-08
   
   Inputs:
    mm (mmap): memory map of wftest output file e.g. wftest.000
    header (str): 'Deriv' or 'Hderiv'
    nmax_deriv (int): maximum number of parameters to parse
   Returns:
    dict: derivative data in dictionary having keys ('iparam','numeric','analytic','diff')
  """

  idx = mm.find(header)
  if idx == -1:
    raise RuntimeError('failed to find %s'%header)
  mm.seek(idx)
  mm.readline()

  data = {'iparam':[],'numeric':[],'analytic':[],'diff':[]}
  for ider in range(nmax_deriv):
    tokens = mm.readline().split()
    if len(tokens) < 4:
      break # reached the end of data block
    if ider >= nmax_deriv:
      raise RuntimeError('please increase nmax_deriv')
    iparam = int( tokens[0] )
    numeric,analytic,diff = map(float,tokens[1:])
    for name,val in zip(data.keys(),[iparam,numeric,analytic,diff]):
      data[name].append(val)
    # end for 
  # end for ider
  return data
# end def parse_deriv_block

def check_relative_error(data,rel_tol=1e-3,eps=1e-16):
  # data should have 4 arrays saved in keys: ('iparam','numeric','analytic','diff')
  success = True
  par_list = data['iparam']
  num_list = data['numeric']
  ana_list = data['analytic']
  diff_list= data['diff']
  for iparam in range(len(par_list)):
    num  = num_list[iparam]
    if abs(num) < eps:
      continue # skip 0 denominator
    ana  = ana_list[iparam]
    diff = diff_list[iparam]
    rel_err = diff/abs(num)
    if rel_err > rel_tol:
      print('relative error for parameter %d is %f, which exceeds tolerance %f' % (par_list[iparam],rel_err,rel_tol))
      success = False
    # end if
  # end for
  return success


if __name__ == '__main__':
  fname = 'wftest.000'
  rel_tol = 1e-3

  from mmap import mmap
  with open(fname,'r+') as f:
    mm = mmap(f.fileno(),0)
  # end with

  # parse overlap matrix derivatives
  odata = parse_deriv_block(mm,'Deriv')
  # parse hamiltonian matrix derivatives
  hdata = parse_deriv_block(mm,'Hderiv')

  # check analytic derivatives against numerical derivatives
  opass = check_relative_error(odata)
  hpass = check_relative_error(hdata)

  if opass and hpass:
    exit(0)
  else:
    exit(1)

# __main__
