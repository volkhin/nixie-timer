#!/bin/bash

PROTO_COMMAND="$HOME/Downloads/nanopb-0.3.9.2-macosx-x86/generator-bin/protoc"

all_proto_files=$(find ./proto/ -name "*.proto")
#echo $all_proto_files

for file in $all_proto_files; do
  file_path=${file%/*}
  echo "filepath: $file_path"

	#Filter out the duplicate paths here TODO
	input_path="-I=$file_path/ "
        #Use the below for output files in the same folders as input folders
	#output_path="--nanopb_out=-I$file_path/:$file_path/ "
	output_path="--nanopb_out=-I$file_path/:./src/"
        #echo $output_path
	proto_path="$file_path/*.proto"

	#Run the proto compilation
  cmd="$PROTO_COMMAND -I=./proto/common/ $input_path $output_path $proto_path"
  echo $cmd
  $cmd
  echo ""
done
