export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export G4SIMEXEC=$MYG4SIMROOT/bin/g4sim
if ! echo $PATH | grep -q "$MYG4SIMROOT"; then
	export PATH=$MYG4SIMROOT/build:$PATH
fi
