#!/usr/bin/bash

createviews() {
    cat <<'EOF'
create temporary view PhotoVideoView as
select p.id as photoid, p.event_id as event_id, p.exposure_time as exposure_time, p.filename as filename, 'photo' as type from PhotoTable as p
union all
select v.id as photoid, v.event_id as event_id, v.exposure_time as exposure_time, v.filename as filename, 'video' as type from VideoTable as v;

create temporary view EventViewWithStartTime as
select epv.photoid as photoid, epv.event_id as event_id, epv.name as name, epv.exposure_time as event_time from (		
    select pv.photoid, pv.event_id, e.name, pv.exposure_time from PhotoVideoView pv
	left outer join EventTable as e on pv.event_id = e.id order by exposure_time desc
) epv group by event_id order by exposure_time;
EOF
}

gendefvarquery() {
    printf 'select "const int %s = " || count(*) || ";" from %s where %s;\n' "$1" "$2" "$3"
}

gendefvarquerydate() {
    printf -v condition 'strftime("%%Y", event_time, "unixepoch") = "%s"' "$3"

    if [[ -n $4 ]]; then
        printf -v condition '%s and strftime("%%m", event_time, "unixepoch") = "%s"' "$condition" "$4"
    fi

    gendefvarquery "$1" "$2" "$condition"
}

printf '#ifndef SHOTWINTESTDATA_H\n'
printf '#define SHOTWINTESTDATA_H\n\n'

{
createviews

gendefvarquery photoTotal PhotoVideoView 1=1
gendefvarquery eventTotal EventTable 1=1

gendefvarquerydate eventsInApril2017 EventViewWithStartTime 2017 04
gendefvarquerydate eventsIn2016 EventViewWithStartTime 2016

} | sqlite3 "$1"


printf '\n#endif\n'
