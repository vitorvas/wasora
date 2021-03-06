rm -f version.h

if test -e ../.git -a ! -z "`which git`"; then
 version=`git describe --tags | sed 's/-/./'`
#  echo "[[define]](wasoraversion, ${version})[[dnl]]" > ../version.m4

 branch=$(git symbolic-ref HEAD | sed -e 's,.*/\(.*\),\1,')
 date=`git log --pretty=format:"%ad" | head -n1`
 cat << EOF > version-vcs.h
#define WASORA_VCS_BRANCH    "${branch}"
#define WASORA_VCS_VERSION   "${version}"
#define WASORA_VCS_DATE      "${date}"
#define WASORA_VCS_CLEAN     `git status --porcelain | wc -l`
EOF
fi

cat version-vcs.h >> version.h

if [ -e version-conf.h ]; then
  cat version-conf.h >> version.h
fi

cat << EOF >> version.h
#define COMPILATION_DATE     "`date +'%Y-%m-%d %H:%M:%S'`"
#define COMPILATION_USERNAME "`whoami | sed s/\\\\\\\\//`"
#define COMPILATION_HOSTNAME "`hostname`"
#define WASORA_DATE          "`stat -c %y *.c mesh/*.c | sort -r | head -n1 | cut -b-19`"
#define WASORA_HEADERMD5     "`md5sum wasora.h | cut -c-32`"
EOF
