echo -n "Checking sha3-256"
fail=no
for f in Test/testfile/*;do
    opensslres=$(openssl dgst -sha3-256 $f | sed 's/.*= //');
    myshares=$(./SHA3-256 < $f)
    echo "Openssl  SHA3-256($f) -> $opensslres"
    echo "My_SHA3-256($f) -> $myshares"
    if [ "$myshares" = "$opensslres" ]; then
        echo "Same hash my_sha3-256($f) : OK";
    else
        echo "Different hash my_sha3-256($f) : KO";
        fail=yes;
    fi
    echo "";
done

if [ "$fail" = "no" ]; then
    echo "My_SHA3-256 seem to be valid =)";
else
    echo "My_SHA3-256 is invalid =(";
    exit 1
fi

echo "\n Starting Benchmark \n"
total=0
for i in $(seq 0 100); do
    res=$({ time ./SHA3-256 < Test/testfile/big 1>/dev/null; } 2>&1);
    sec=$(echo $res | head -c 27 | tail -c 2);
    num=$(( $num + $sec ));
done
avg=$(( $num / 100))
bps=$(( 10 / $avg * 8519680))
echo "My sha3 hash with a rate of $bps Bytes per second"