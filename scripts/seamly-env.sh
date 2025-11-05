#!/usr/bin/env bash
#
# Helper script to configure the environment for building Seamly2D without root.
# Source this file (do not execute) before running qmake/make or the binaries.
#
#   source scripts/seamly-env.sh
#

set -euo pipefail

QT_VERSION="${QT_VERSION:-6.7.2}"
QT_ROOT="${QT_ROOT:-$HOME/Qt/${QT_VERSION}/gcc_64}"

if [[ ! -d "${QT_ROOT}" ]]; then
    echo "Expected Qt installation not found at ${QT_ROOT}" >&2
    return 1 2>/dev/null || exit 1
fi

add_path() {
    local dir="$1"
    local target_var="$2"
    if [[ -d "${dir}" ]]; then
        if [[ -z "${!target_var:-}" ]]; then
            printf -v "${target_var}" "%s" "${dir}"
        elif [[ ":${!target_var}:" != *":${dir}:"* ]]; then
            printf -v "${target_var}" "%s:%s" "${dir}" "${!target_var}"
        fi
        export "${target_var}"
    fi
}

add_path "${QT_ROOT}/bin" PATH
add_path "$HOME/.local/bin" PATH

add_path "${QT_ROOT}/lib" LD_LIBRARY_PATH
add_path "$HOME/.local/lib" LD_LIBRARY_PATH
add_path "$HOME/.local/usr/lib" LD_LIBRARY_PATH
add_path "$HOME/.local/usr/lib/x86_64-linux-gnu" LD_LIBRARY_PATH
add_path "$HOME/.local/usr/lib/x86_64-linux-gnu/pulseaudio" LD_LIBRARY_PATH

add_path "$HOME/.local/include" CPLUS_INCLUDE_PATH
add_path "$HOME/.local/usr/include" CPLUS_INCLUDE_PATH

add_path "$HOME/.local/lib" LIBRARY_PATH
add_path "$HOME/.local/usr/lib" LIBRARY_PATH
add_path "$HOME/.local/usr/lib/x86_64-linux-gnu" LIBRARY_PATH
add_path "$HOME/.local/usr/lib/x86_64-linux-gnu/pulseaudio" LIBRARY_PATH

export QT_ROOT
export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-xcb}"

echo "Seamly2D environment configured."
