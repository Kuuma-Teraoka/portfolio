#!/usr/bin/env bash


help="\
Usage: run [OPTIONS] hello.ts
Usage: run [OPTIONS] matpri.cpp -t w_x.fmat

OPTIONS:
  -m: make binary to \$RUNNER_BIN_PATH/ (such as \$BUILD_PATH/dist/handmade/bin/)
  -l: compile \$RUNNER_CPP_LIBCPP_PATH/*.cpp (such as cpp_library_handmade/*.cpp)
"


# 依存関係であるsafety_remover.pyという実行可能なプログラムがあるか確認
if ! which safety_remover.py > /dev/null
then
    echo "safety_remover.py not found"
    echo "please install safety_remover.py"
    exit 1
fi

while getopts "ml" opt
do
    case $opt in
        m)
            m=true
            ;;
        l)
            l=true 
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done

shift $(($OPTIND-1))

if [ $# -eq 0 ]
then
    echo "$help" | head -c -1
    exit 1
fi

if [[ "$1" == *.tex ]]
then
  if [ -z "$RUNNER_BUILD_PATH" ]
  then
    echo "\$RUNNER_BUILD_PATH is not set."
    exit 1 
  fi

  target="${1%.tex}.pdf"

  (set -x ; lualatex -output-directory=$RUNNER_BUILD_PATH $1)
  (set -x ; cp $RUNNER_BUILD_PATH/$(basename ${1%.tex}).pdf $target)
  echo "generated $target"



elif [[ "$1" == *.ts ]]
then
  if [ -z "$RUNNER_BUILD_PATH" ]
  then
    echo "\$RUNNER_BUILD_PATH is not set."
    exit 1
  elif [ -z "$RUNNER_BIN_PATH" ]
  then
    echo "\$RUNNER_BIN_PATH is not set."
    exit 1
  fi

  if [ -n "$m" ]
  then
    target_binary="$(basename ${1%.ts}).js"
  else
    target_binary="runner_tmprunner_tmp.js"
  fi

  (set -x ; tsc $1 --outDir $RUNNER_BUILD_PATH)
  echo "generated $RUNNER_BUILD_PATH/$(basename ${1%.ts}).js"

  echo "#!/usr/bin/env node" > $RUNNER_BIN_PATH/$target_binary
  cat $RUNNER_BUILD_PATH/$(basename ${1%.ts}).js >> $RUNNER_BIN_PATH/$target_binary
  chmod 755 $RUNNER_BIN_PATH/$target_binary 
  safety_remover.py $RUNNER_BUILD_PATH/$(basename ${1%.ts}).js
  echo "generated js file: $RUNNER_BIN_PATH/$target_binary"
  
  if [ -z "$m" ]
  then
    (set -x; $RUNNER_BIN_PATH/$target_binary ${@:2})
  fi






elif [[ "$1" == *.java ]]
then
    REQUIRED_ENV_VARS=( 
        RUNNER_JAVA_LIB_PATH
        RUNNER_BUILD_PATH
        RUNNER_BIN_PATH
        RUNNER_LIBEXEC_PATH
        RUNNER_LIB_JAR_PATH
        JAVA_HOME
    )
    
    for i in "${REQUIRED_ENV_VARS[@]}"
    do
        if [ -z "${!i}" ]
        then
            echo "\$$i is not set."
            exit 1
        fi
    done
 

    if [ -n "$m" ]
    then
        echo "sorry, this option is not support yet."
        exit 1
    fi
 
    if [ -n "$l" ]
    then
        safety_remover.py $RUNNER_BUILD_PATH/jar
        mkdir $RUNNER_BUILD_PATH/jar
        (set -x ; javac -d $RUNNER_BUILD_PATH/jar $RUNNER_JAVA_LIB_PATH/*.java)
        (set -x ; jar cf $RUNNER_LIB_JAR_PATH/lib_java_handmade.jar -C $RUNNER_BUILD_PATH/jar ./)
    fi

    (set -x ; javac -d $RUNNER_BUILD_PATH $1)
    (set -x ; java -cp $CLASSPATH:$RUNNER_BUILD_PATH $(basename ${1%.java}))











elif [[ "$1" == *.cpp ]]
then
  REQUIRED_ENV_VARS=(
    RUNNER_CPP_LIBHPP_PATH
    RUNNER_CPP_LIBCPP_PATH
    RUNNER_BUILD_PATH
    RUNNER_BIN_PATH
    RUNNER_INCLUDE_PATH
    RUNNER_LIB_PATH
  )
  
  for i in "${REQUIRED_ENV_VARS[@]}"
  do
    if [ -z "${!i}" ]
    then
      echo "\$$i is not set."
      exit 1
    fi
  done


  if [ -n "$m" ]
  then
    target_binary="$(basename ${1%.cpp})"
  else
    target_binary="runner_cpp_temp_binary"
  fi

  if [ -z "$CXX" ]
  then
    CXX="g++"
  fi

  #rvo_none="-std=c++17 -fno-elide-constructors"
  newest="-std=c++20"
  speed='-O3'

  #debug="-g -O0" 
  #gdb='gdb'
  #add_san='-fsanitize=address' # address sanitizer

  lapack_opt="-llapacke -lopenblas -lgfortran"
  #boost='-lboost_iostreams -lboost_system'
  curl='-lcurl'

  if [ -n "$l" ]
  then
    (set -x ; rsync -avu --include='*.hpp' --exclude='*' $RUNNER_CPP_LIBHPP_PATH/ $RUNNER_INCLUDE_PATH/)
    safety_remover.py $RUNNER_BUILD_PATH/*
    find $RUNNER_CPP_LIBCPP_PATH -name "*.cpp" | while IFS= read -r i
    do
      o_file="$RUNNER_BUILD_PATH/$(basename ${i%.cpp}).o"
      (set -x ; $CXX $newest -c $i -o $o_file)
    done
    (set -x ; ar rcs $RUNNER_LIB_PATH/libhandmade.a $RUNNER_BUILD_PATH/*)
  fi

  safety_remover.py $RUNNER_BIN_PATH/$target_binary
  (set -x ; $CXX $debug $add_san $rvo_none $newest $speed -o $RUNNER_BIN_PATH/$target_binary "$1" $lapack_opt $boost $curl -lhandmade)
  (set -x ; $gdb $target_binary ${@:2})
fi











