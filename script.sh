#!/bin/bash

# Check argument number
if [ "$#" -ne 1 ]; then
    echo "Argumente incorecte"
    exit 1
fi


char="$1"

counter=0

function is_valid_sentence {
    local sentence="$1"
    
    # Check start upper case
    if [[ ! "$sentence" =~ ^[A-Z] ]]; then
        return 1
    fi

    # Check valid chars
    if [[ ! "$sentence" =~ ^[A-Za-z0-9 ,.!?]*$ ]]; then
        return 1
    fi

    # Check end of sentence
    if [[ ! "$sentence" =~ [?.!]$ ]]; then
        return 1
    fi

    # Check comma
    if [[ "$sentence" =~ ,.*si ]]; then
        return 1
    fi

    return 0
}

while IFS= read -r line; do
    # Check if character present
    if [[ "$line" == *"$char"* ]]; then
        if is_valid_sentence "$line"; then
            ((counter++))
        fi
    fi
done

echo "$counter"
