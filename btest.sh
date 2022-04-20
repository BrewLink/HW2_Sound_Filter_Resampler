# Basic Freq Test Script

#!bash

# make fucnction 
build_halfrate() {
  make clean && make
}

# run the main file 
run_down_sampler() {
  echo "Running downsmapler on $1"
  
  ./halfrate $1
}

play_analyze(){
  echo "Analyzing processed file $1"
  # Play and log the freqs
  play $1 stat -freq >& freq.txt
}

process_output_file(){
  # Clean the file to just data
  sed -i "" '1,11d' $1
  sed -i "" '/^$/d' $1
  sed -i "" '/In/d' $1
  for i in {1..17}
  do
    sed -i "" '$d' $1
  done
}

check_max_freq(){
  echo "Finding Max Value..."
  awk -v max=0 '{if($1>max){max=$1}}END{print "Max Freq Found: "; print max}' $1
}


check_min_freq(){
  echo "Finding Min Value..."

  awk 'NR == 1 {line = $0; min = $1}
      NR > 1 && $1 < min {line = $0; min = $1}
      END{print min}' $1 
}

songs=("./org_songs/fifths.wav" "./org_songs/sine.wav" "./org_songs/gc.wav"
"./org_songs/synth.wav")
rsongs=("./org_songs/rfifths.wav" "./org_songs/rsine.wav" "./org_songs/rgc.wav"
"./org_songs/rsynth.wav")
output_file="freq.txt"

# Rebuild
build_halfrate

index=0
echo "\n Running Analysis on Unmodified Songs------------"
# Run rsong max & min freq check
for str in ${songs[@]}; 
do
  echo "Test index $index --------------------------------"
  run_down_sampler $str
  play_analyze ${rsongs[$index]}
  process_output_file $output_file
  check_max_freq $output_file
  check_min_freq $output_file
  let "index=index+1"
  echo "--------------------------------------------------"
done


echo "\n Running Analysis on Unmodified Songs------------"
# Run song max & min freq check
for str in ${songs[@]}; 
do
  echo "Test index $index --------------------------------"
  play_analyze $str
  process_output_file $output_file
  check_max_freq $output_file
  check_min_freq $output_file
  let "index=index+1"
  echo "--------------------------------------------------"
done


exit  
