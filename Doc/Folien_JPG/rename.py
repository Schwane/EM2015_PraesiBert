import os


dir_ents = os.listdir('.')
print dir_ents

fn = "Folie1.JPG"
i = 0
while fn in dir_ents:
	os.rename(fn, "%03d.jpg"%i)
	i += 1
	fn = "Folie%d.JPG"%(i+1)