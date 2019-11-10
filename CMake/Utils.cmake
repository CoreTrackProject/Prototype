function(add_source)
    get_property(tmp GLOBAL PROPERTY source_list)
	
    foreach(arg ${ARGV})

		list(APPEND tmp ${arg})
    endforeach()
	
    set_property(GLOBAL PROPERTY source_list ${tmp})
endfunction(add_source)


function(add_include)
	get_property(tmp GLOBAL PROPERTY include_list)

	foreach(arg ${ARGV})

		list(APPEND tmp ${arg})
    endforeach()
	
	set_property(GLOBAL PROPERTY include_list ${tmp})
endfunction(add_include)
