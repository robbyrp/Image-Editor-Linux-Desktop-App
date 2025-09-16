pushd .. &> /dev/null

zip -r cs.zip \
    cs/README.md \
    cs/cs.sh  \
    cs/checkpatch.pl \
    cs/spelling.txt  \
    cs/const_structs.checkpatch

popd &> /dev/null
