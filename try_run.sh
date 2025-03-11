while true; do
	resman -x 'sh tests-cifar10-probe.sh' > tests-cifar10-probe.log
	EXIT_CODE=$?

	if [ $EXIT_CODE -eq 0 ]; then
		exit 0
	fi

	sleep 300
done
