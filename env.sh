export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export G4SIMEXEC=$MYG4SIMROOT/bin/g4sim
if ! echo $LD_LIBRARY_PATH | grep -q "$MYG4SIMROOT"; then
	export LD_LIBRARY_PATH=$MYG4SIMROOT/lib:$LD_LIBRARY_PATH
fi
if ! echo $PATH | grep -q "$MYG4SIMROOT"; then
	export PATH=$MYG4SIMROOT/bin:$PATH
fi
