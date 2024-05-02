for g in 0 -10 -50 10; do
    for j in 0 -5 -15; do
        for m in 0 0.5 1.0 2; do
            ./make_average.py autotests_30apr/data/*"_${g}_${j}_${m}_"* > ./autotests_30apr_avgs/"AVG_${g}_${j}_${m}.json"
        done
    done
done
