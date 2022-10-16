test_file=Makefile
test_phrase="this is a very long phrase that should give a hash, and this is also very tiring to find word to write in this sentence just for a"

echo -n "Checking Cksum for a file : "
./keccak -v -224 $test_file -256 $test_file -384 $test_file -512 $test_file> Test/resultcksum
diff Test/reffile Test/resultcksum > Test/problemfile;
if [ $? -eq 0 ]; 
then
    echo "ok";
    rm Test/problemfile;
else
    echo "ko check problemfile for more information";
fi
rm Test/resultcksum
echo -n "Checking string hash: "
./keccak -v -224 $test_phrase -256 $test_phrase -384 $test_phrase -512 $test_phrase > Test/resultstr;
diff Test/refstr Test/resultstr > Test/problemstr ;
if [ $? -eq 0 ]; 
then
    echo "ok";
    rm Test/problemstr;
else
    echo "ko check problemstr for more information";
fi
rm Test/resultstr

if [ -f Test/problemstr ] || [ -f Test/problemfile ]; then
    echo "Test Failed";
else
    echo "Test succeed";
fi