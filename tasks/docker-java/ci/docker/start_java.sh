#!/bin/bash
set -e

# Proxies section
if [ ! -z "${HTTP_PROXY}" ]; then
	export http_proxy=$HTTP_PROXY

	HTTP_PROXY_SCHEME=$(echo $HTTP_PROXY | awk -F '://' '{print $1}' )
	HTTP_PROXY_HOST=$(echo $HTTP_PROXY | awk -F '://' '{print $2}' | awk -F ':' '{print $1}')
	HTTP_PROXY_PORT=$(echo $HTTP_PROXY | awk -F ':' '{print $NF}' | grep -v '\/\/')

	if [ "$HTTP_PROXY_PORT" = "" ]; then
		HTTP_PROXY_PORT="80"
	fi
fi

if [ ! -z "${HTTPS_PROXY}" ]; then
	export https_proxy=$HTTPS_PROXY

	HTTPS_PROXY_SCHEME=$(echo $HTTPS_PROXY | awk -F '://' '{print $1}' )
	HTTPS_PROXY_HOST=$(echo $HTTPS_PROXY | awk -F '://' '{print $2}' | awk -F ':' '{print $1}')
	HTTPS_PROXY_PORT=$(echo $HTTPS_PROXY | awk -F ':' '{print $NF}' | grep -v '\/\/')

	if [ "$HTTPS_PROXY_PORT" = "" ]; then
		HTTPS_PROXY_PORT="443"
	fi
fi

proxies=""
if [ ! -z "${HTTP_PROXY_HOST}" ]; then
	proxies=$proxies"-Dhttp.proxyHost=${HTTP_PROXY_HOST} "
fi

if [ ! -z "${HTTP_PROXY_PORT}" ]; then
	proxies=$proxies"-Dhttp.proxyPort=${HTTP_PROXY_PORT} "
fi

if [ ! -z "${HTTPS_PROXY_HOST}" ]; then
	proxies=$proxies"-Dhttps.proxyHost=${HTTPS_PROXY_HOST} "
fi


if [ ! -z "${NO_PROXY_HOSTS}" ]; then
	NO_PROXY_HOSTS=${NO_PROXY_HOSTS//,/|}
	NO_PROXY_HOSTS=${NO_PROXY_HOSTS// /}
	proxies=$proxies"-Dhttp.nonProxyHosts=\"${NO_PROXY_HOSTS}\" "
fi

# Newrelic section
# If https proxy exists overwrite http proxy for newrelic
newrelic=""
if [ "${NEW_RELIC_AGENT_ENABLED}" = "true" ]; then
	newrelic="-javaagent:/newrelic.jar"

	if [ ! -z "${HTTP_PROXY_HOST}" ]; then
		export NEW_RELIC_PROXY_HOST=${HTTP_PROXY_HOST}
	elif [ ! -z "${NEW_RELIC_HTTP_PROXY_HOST}" ]; then
		export NEW_RELIC_PROXY_HOST=${NEW_RELIC_HTTP_PROXY_HOST}
	fi

	if [ ! -z "${HTTPS_PROXY_HOST}" ]; then
		export NEW_RELIC_PROXY_HOST=${HTTPS_PROXY_HOST}
	elif [ ! -z "${NEW_RELIC_HTTPS_PROXY_HOST}" ]; then
		export NEW_RELIC_PROXY_HOST=${NEW_RELIC_HTTPS_PROXY_HOST}
	fi

	if [ ! -z "${HTTP_PROXY_SCHEME}" ]; then
		export NEW_RELIC_PROXY_SCHEME=${HTTP_PROXY_SCHEME}
	fi

	if [ ! -z "${HTTPS_PROXY_SCHEME}" ]; then
		export NEW_RELIC_PROXY_SCHEME=${HTTPS_PROXY_SCHEME}
	fi

	if [ ! -z "${HTTP_PROXY_PORT}" ]; then
		export NEW_RELIC_PROXY_PORT=${HTTP_PROXY_PORT}
	elif [ ! -z "${NEW_RELIC_HTTP_PROXY_PORT}" ]; then
		export NEW_RELIC_PROXY_PORT=${NEW_RELIC_HTTP_PROXY_PORT}
	fi

	if [ ! -z "${HTTPS_PROXY_PORT}" ]; then
		export NEW_RELIC_PROXY_PORT=${HTTPS_PROXY_PORT}
	elif [ ! -z "${NEW_RELIC_HTTPS_PROXY_PORT}" ]; then
		export NEW_RELIC_PROXY_PORT=${NEW_RELIC_HTTPS_PROXY_PORT}
	fi
fi

debug_gc=""

if [ "${DEBUG_GC}" = "true" ]; then
	debug_gc="-XX:+PrintGCDetails -XX:+PrintGCDateStamps - Xloggc:/tmp/"
fi

memory_options="-XX:+UnlockExperimentalVMOptions -XX:MaxRAMPercentage=70.0 -XX:MinRAMPercentage=50.0 -XX:+HeapDumpOnOutOfMemoryError -XX:+UseCGroupMemoryLimitForHeap -XshowSettings:vm"

if [ -z "${JVM_MEM_ARGS}" ]; then
	memory_options=${JVM_MEM_ARGS}
fi

pwd

echo "java $proxies $debug_gc $memory_options -Djava.security.egd=file:/dev/./urandom  $newrelic $@"
exec java $proxies $debug_gc $memory_options -Djava.security.egd=file:/dev/./urandom  $newrelic -jar /app.jar $@

