SECTIONS
{
  .__lte_evts ADDR(.text) + SIZEOF(.text) + SIZEOF(.data) : 
  {
    PROVIDE (__start___lte_evts = . );
    *(__lte_evts*)
    PROVIDE (__stop___lte_evts = . );
  }>text
}



