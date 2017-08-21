# identify -verbose $file_path | grep -E 'Delay|Alpha: srgba'

# convert ~/Desktop/spikedcheepcheep.gif -depth 8 -alpha off txt:-

for file in ./gifs/*
do
  delay=`identify -verbose $file | grep -m 1 "Delay:" | grep -oh "\d*x\d*"`
  if [ -z "$delay" ]; then
    delay='1x100'
  fi
  s=${file##*/}
  echo "delay:$delay" > ./data/${s%.*}.txt
  convert $file -background black -alpha remove txt:- >> ./data/${s%.*}.txt
  echo "# Next image" >> ./data/${s%.*}.txt
done
