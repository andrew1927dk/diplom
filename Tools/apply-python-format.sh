#!/usr/bin/env bash

staged=${1:-true}
apply_to_staged=${2:-true}


function error_exit() {
    for str in "$@"; do
        echo -n "$str" >&2
    done
    echo >&2

    exit 1
}

function remove_non_ascii() {
    while read -r data; do
      echo "$data" | tr -cd '\11\12\15\40-\176'
    done
}

function check_yq() {
  if ! which yq &> /dev/null; then
    error_exit $'Please install yq \n>> sudo wget https://github.com/mikefarah/yq/releases/latest/download/yq_linux_amd64 -O /usr/local/bin/yq && sudo chmod +x /usr/local/bin/yq'
  fi
}

function python_executable() {
  local python_files
  python_files=$1

  for file in $python_files; do
    if grep -q -E "if __name__\ *?==\ *['\"]__main__['\"]:$" "$file"; then
      echo "Attempting to run python file $file"
      if [[ $(head -n 1 "$file") != '#!/usr/bin/env python3' ]]; then
        if $apply_to_staged; then
          echo "$(echo '#!/usr/bin/env python3'; cat "$file")" > "$file";
          git add "$file";
        else
          error_exit "Add shebang to file: $file";
        fi;
      fi;
      if [[ ! -x $file ]]; then
        if $apply_to_staged; then
          chmod +x "$file";
          git add "$file";
        else
          error_exit "$file is not executable!";
        fi;
      fi;
    fi;
  done
}

function python_formatter() {
  local python_files
  python_files=$1
  if [[ -n "$python_files" ]]; then
    echo "Applying Python format to staged files";
    if which black &> /dev/null; then
      if [[ $apply_to_staged ]]; then
        sh -c "black --line-length 120 --skip-string-normalization $python_files" 2>&1 | remove_non_ascii 1>&2;
        sh -c "git add $python_files";
      else
        sh -c "black --line-length 120 --skip-string-normalization --check $python_files" 2>&1 | remove_non_ascii 1>&2;
      fi;
    else
      error_exit $'\nYou need to install black!\n>> pip3 install black'
    fi
  fi
}

function check_farm_config() {
  local changed_files output1 output2
  changed_files=$1;
  output1=$(mktemp);
  output2=$(mktemp);

  check_yq;
  yq '.JENKINS_CONFIGS.templates | join("\n")' devops/global_vars.yaml > "$output1";
  echo "$changed_files" > "$output2";

  if grep -qFxf "$output1" "$output2"; then
    echo "Applying Jenkins configs";
    if ! python3 -c "import bs4, requests" 2> /dev/null; then
      error_exit $'Please install beautifulsoup4 and requests\n>> python3 -m pip install beautifulsoup4 requests'
    fi
    if [[ -z "$AWS_JENKINS_USER$AWS_JENKINS_TOKEN" ]]; then
        echo "WARNING! Jenkins credentials is not configured! Check \$AWS_JENKINS_USER and \$AWS_JENKINS_TOKEN variables" >&2 ;
    else
      if ! ./install/ci/check_config_farm_configs.py devops/global_vars.yaml; then
        rm -f "$output1" "$output2";
        echo "WARNING! Farm config(s) is not actual. Please update!" >&2;
      fi
    fi
  fi
  rm -f "$output1" "$output2";
}

BITBUCKET_PR_DESTINATION_BRANCH="${BITBUCKET_PR_DESTINATION_BRANCH:-}"
params=''
if $staged; then
  params='--staged'
elif [[ -n "$BITBUCKET_PR_DESTINATION_BRANCH" ]]; then
  params="HEAD $BITBUCKET_PR_DESTINATION_BRANCH"
else
  params=''
fi

changed_files=$(sh -c "git diff $params --diff-filter=ARMCT --name-only")
changed_python_files=$(echo "$changed_files" | grep -E '.*\.py'| tr '\n' ' ' | xargs echo || echo '')
changed_devops_python_files=$(echo "$changed_python_files" | tr ' ' '\n' | grep -E '^devops/' | tr '\n' ' ')

python_executable "$changed_python_files"
python_formatter "$changed_devops_python_files"
check_farm_config "$changed_files"
