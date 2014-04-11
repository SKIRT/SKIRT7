pro ConvertMappings

restore, 'Cparam_models.save', /verbose  

Zname = ['Z005','Z020','Z040','Z100','Z200']
logCname = ['C40','C45','C50','C55','C60','C65']
logpname = ['p4','p5','p6','p7','p8']

Nlambda = 1800
lambdav = dblarr(Nlambda)
flux0v = dblarr(Nlambda)
flux1v = dblarr(Nlambda)

; invert the wavelength vector and convert from micron to m

for ell=0,Nlambda-1 do begin
  lambdav[ell] = wave_micron[Nlambda-1-ell]/1e6
endfor


for i=0,4 do begin
  for j=0,5 do begin
    for k=0,4 do begin
      filename = 'Mappings_' + Zname[i] + '_' + logCname[j] + '_' + logpname[k] + '.dat'
      print, filename
      for ell=0,Nlambda-1 do begin
	flux0v[ell] = model_spectra[i,j,k,0,Nlambda-1-ell]
	flux1v[ell] = model_spectra[i,j,k,1,Nlambda-1-ell]
	if (flux0v[ell] LE 1e-40) then flux0v[ell] = 0.0
	if (flux1v[ell] LE 1e-40) then flux1v[ell] = 0.0
      endfor

; conversion from erg/s/Hz to W/m

      flux0v = flux0v * 1e-7 * 2.99792458e8 / lambdav / lambdav
      flux1v = flux1v * 1e-7 * 2.99792458e8 / lambdav / lambdav

      writecol, filename, lambdav, flux0v, flux1v
    endfor
  endfor
endfor


return
end