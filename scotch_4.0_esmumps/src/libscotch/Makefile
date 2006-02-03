## Copyright INRIA 2004
##
## This file is part of the Scotch distribution.
##
## The Scotch distribution is libre/free software; you can
## redistribute it and/or modify it under the terms of the
## GNU Lesser General Public License as published by the
## Free Software Foundation; either version 2.1 of the
## License, or (at your option) any later version.
##
## The Scotch distribution is distributed in the hope that
## it will be useful, but WITHOUT ANY WARRANTY; without even
## the implied warranty of MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE. See the GNU Lesser General Public
## License for more details.
##
## You should have received a copy of the GNU Lesser General
## Public License along with the Scotch distribution; if not,
## write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
##
## $Id$
##

##
##  General inference rules.
##

%$(OBJ)	:	%.c
				$(CC) $(CFLAGS) -c $(<) -o $(@)

%$(EXE)	:	%.c
		 		$(CC) $(CFLAGS) $(<) -o $(@) $(LDFLAGS)

##
##  Project rules.
##

include ../Makefile.inc

.PHONY				:	all

all				:	scotch.h				\
					scotchf.h				\
					libscotch$(LIB)				\
					libscotcherr$(LIB)			\
					libscotcherrcom$(LIB)

install				:	all
					-$(CP) scotch.h scotchf.h libscotch$(LIB) libscotcherr$(LIB) libscotcherrcom$(LIB) ../../bin

clean				:
					-$(RM) *~ *$(OBJ) lib*$(LIB) common2* parser_yy.c parser_ly.h parser_ll.c scotch.h scotchf.h y.output dummysizes$(EXE)

realclean			:	clean

##
##  Todo list.
##

common$(OBJ)			:	common.c				\
					module.h				\
					common.h
					$(CAT) module.h common.c > common2.c
					$(CC) $(CFLAGS) -c common2.c -DSCOTCH_COMMON_RENAME -o $(@)
					$(RM) common2.c

common_integer$(OBJ)		:	common_integer.c			\
					module.h				\
					common.h				\
					common_integer.h
					$(CAT) module.h common_integer.c > common2_integer.c
					$(CC) $(CFLAGS) -c common2_integer.c -DSCOTCH_COMMON_RENAME -o $(@)
					$(RM) common2_integer.c

common_memory$(OBJ)		:	common_memory.c				\
					module.h				\
					common.h
					$(CAT) module.h common_memory.c > common2_memory.c
					$(CC) $(CFLAGS) -c common2_memory.c -DSCOTCH_COMMON_RENAME -o $(@)
					$(RM) common2_memory.c

arch$(OBJ)			:	arch.c					\
					module.h				\
					common.h				\
					arch.h					\
					arch_cmplt.h				\
					arch_deco.h				\
					arch_hcub.h				\
					arch_mesh.h				\
					arch_tleaf.h				\
					arch_torus.h				\
					arch_vcmplt.h				\
					arch_vhcub.h

arch_build$(OBJ)		:	arch_build.c				\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					arch_deco.h				\
					bgraph.h				\
					bgraph_bipart_st.h			\
					arch_build.h

arch_cmplt$(OBJ)		:	arch_cmplt.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_cmplt.h

arch_deco$(OBJ)			:	arch_deco.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_deco.h

arch_hcub$(OBJ)			:	arch_hcub.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_hcub.h

arch_mesh$(OBJ)			:	arch_mesh.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_mesh.h

arch_tleaf$(OBJ)		:	arch_tleaf.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_tleaf.h

arch_torus$(OBJ)		:	arch_torus.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_torus.h

arch_vcpmlt$(OBJ)		:	arch_vcmplt.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_vcmplt.h

arch_vhcub$(OBJ)		:	arch_vhcub.c				\
					module.h				\
					common.h				\
					arch.h					\
					arch_vhcub.h

gain$(OBJ)			:	gain.c					\
					module.h				\
					common.h				\
					gain.h

graph$(OBJ)			:	graph.c					\
					module.h				\
					common.h				\
					graph.h

graph_base$(OBJ)		:	graph_base.c				\
					module.h				\
					common.h				\
					graph.h

graph_check$(OBJ)		:	graph_check.c				\
					module.h				\
					common.h				\
					graph.h

graph_coarsen$(OBJ)		:	graph_coarsen.c				\
					graph_coarsen_edge.c			\
					module.h				\
					common.h				\
					graph.h

graph_induce$(OBJ)		:	graph_induce.c				\
					module.h				\
					common.h				\
					graph.h					\
					graph_induce.h

graph_io$(OBJ)			:	graph_io.c				\
					module.h				\
					common.h				\
					graph.h					\
					graph_io.h

graph_io_chac$(OBJ)		:	graph_io_chac.c				\
					module.h				\
					common.h				\
					geom.h					\
					graph.h

graph_io_habo$(OBJ)		:	graph_io_habo.c				\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					graph_io_habo.h

graph_io_scot$(OBJ)		:	graph_io_scot.c				\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					graph_io_scot.h

graph_list$(OBJ)		:	graph_list.c				\
					module.h				\
					common.h				\
					graph.h

geom$(OBJ)			:	geom.c					\
					module.h				\
					common.h				\
					geom.h

bgraph$(OBJ)			:	bgraph.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					bgraph.h

bgraph_check$(OBJ)		:	bgraph_check.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					bgraph.h

bgraph_bipart_ex$(OBJ)		:	bgraph_bipart_ex.c			\
					module.h				\
					common.h				\
					gain.h					\
					graph.h					\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_ex.h			\
					bgraph_bipart_fm.h

bgraph_bipart_fm$(OBJ)		:	bgraph_bipart_fm.c			\
					gain.h					\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_fm.h

bgraph_bipart_gg$(OBJ)		:	bgraph_bipart_gg.c			\
					gain.h					\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_gg.h

bgraph_bipart_gp$(OBJ)		:	bgraph_bipart_gp.c			\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_gp.h

bgraph_bipart_ml$(OBJ)		:	bgraph_bipart_ml.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					graph_coarsen.h				\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_ml.h			\
					bgraph_bipart_st.h

bgraph_bipart_st$(OBJ)		:	bgraph_bipart_st.c			\
					gain.h					\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_ex.h			\
					bgraph_bipart_fm.h			\
					bgraph_bipart_gg.h			\
					bgraph_bipart_gp.h			\
					bgraph_bipart_ml.h			\
					bgraph_bipart_st.h			\
					bgraph_bipart_zr.h

bgraph_bipart_zr$(OBJ)		:	bgraph_bipart_zr.c			\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					bgraph.h				\
					bgraph_bipart_zr.h

bgraph_store$(OBJ)		:	bgraph_store.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					bgraph.h

hall_order_hd$(OBJ)		:	hall_order_hd.c				\
					module.h				\
					common.h				\
					graph.h					\
					hall_order_hd.h

hall_order_hf$(OBJ)		:	hall_order_hf.c				\
					module.h				\
					common.h				\
					graph.h					\
					hall_order_hf.h

hall_order_hx$(OBJ)		:	hall_order_hx.c				\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					hall_order_hx.h

hgraph$(OBJ)			:	hgraph.c				\
					module.h				\
					common.h				\
					graph.h					\
					hgraph.h

hgraph_induce$(OBJ)		:	hgraph_induce.c				\
					hgraph_induce_edge.c			\
					module.h				\
					common.h				\
					graph.h					\
					hgraph.h				\
					hgraph_induce.h

hgraph_order_bl$(OBJ)		:	hgraph_order_bl.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hgraph_order_bl.h

hgraph_order_cp$(OBJ)		:	hgraph_order_cp.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hgraph_order_cp.h			\
					hgraph_order_st.h

hgraph_order_gp$(OBJ)		:	hgraph_order_gp.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hgraph_order_gp.h

hgraph_order_hd$(OBJ)		:	hgraph_order_hd.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hall_order_hd.h				\
					hall_order_hx.h				\
					hgraph_order_hd.h			\
					hgraph_order_hx.h			\
					hgraph_order_si.h

hgraph_order_hf$(OBJ)		:	hgraph_order_hf.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hall_order_hf.h				\
					hall_order_hx.h				\
					hgraph_order_hf.h			\
					hgraph_order_hx.h			\
					hgraph_order_si.h

hgraph_order_hx$(OBJ)		:	hgraph_order_hx.c			\
					module.h				\
					common.h				\
					graph.h					\
					hgraph.h				\
					hgraph_order_hx.h

hgraph_order_nd$(OBJ)		:	hgraph_order_nd.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hgraph_order_nd.h			\
					hgraph_order_st.h			\
					vgraph.h				\
					vgraph_separate_st.h

hgraph_order_si$(OBJ)		:	hgraph_order_si.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hgraph_order_si.h

hgraph_order_st$(OBJ)		:	hgraph_order_st.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					graph_coarsen.h				\
					order.h					\
					hgraph.h				\
					vgraph.h				\
					vgraph_separate_st.h			\
					hgraph_order_bl.h			\
					hgraph_order_cp.h			\
					hgraph_order_gp.h			\
					hgraph_order_hd.h			\
					hgraph_order_hf.h			\
					hgraph_order_nd.h			\
					hgraph_order_si.h			\
					hgraph_order_st.h

hmesh$(OBJ)			:	hmesh.c					\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					hmesh.h

hmesh_check$(OBJ)		:	hmesh_check.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					hmesh.h

hmesh_hgraph$(OBJ)		:	hmesh_hgraph.c				\
					module.h				\
					common.h				\
					graph.h					\
					hgraph.h				\
					mesh.h					\
					hmesh.h					\
					hmesh_hgraph.h

hmesh_induce$(OBJ)		:	hmesh_induce.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					hmesh.h

hmesh_mesh$(OBJ)		:	hmesh_mesh.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					hmesh.h

hmesh_order_bl$(OBJ)		:	hmesh_order_bl.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_bl.h			\
					hmesh_order_st.h

hmesh_order_cp$(OBJ)		:	hmesh_order_cp.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_cp.h			\
					hmesh_order_st.h

hmesh_order_gr$(OBJ)		:	hmesh_order_gr.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					hgraph.h				\
					hgraph_order_st.h			\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_gr.h

hmesh_order_gp$(OBJ)		:	hmesh_order_gp.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_gp.h

hmesh_order_hd$(OBJ)		:	hmesh_order_hd.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hall_order_hd.h				\
					hall_order_hx.h				\
					hmesh_order_hd.h			\
					hmesh_order_si.h

hmesh_order_hf$(OBJ)		:	hmesh_order_hf.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hall_order_hf.h				\
					hall_order_hx.h				\
					hmesh_order_hf.h			\
					hmesh_order_si.h

hmesh_order_hx$(OBJ)		:	hmesh_order_hx.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_hx.h

hmesh_order_nd$(OBJ)		:	hmesh_order_nd.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_nd.h			\
					hmesh_order_st.h			\
					vmesh.h					\
					vmesh_separate_st.h

hmesh_order_si$(OBJ)		:	hmesh_order_si.c			\
					module.h				\
					common.h				\
					graph.h					\
					order.h					\
					mesh.h					\
					hmesh.h					\
					hmesh_order_si.h

hmesh_order_st$(OBJ)		:	hmesh_order_st.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					hgraph.h				\
					hgraph_order_st.h			\
					mesh.h					\
					hmesh.h					\
					order.h					\
					vmesh_separate_st.h			\
					hmesh_order_bl.h			\
					hmesh_order_gp.h			\
					hmesh_order_gr.h			\
					hmesh_order_hd.h			\
					hmesh_order_hf.h			\
					hmesh_order_nd.h			\
					hmesh_order_si.h			\
					hmesh_order_st.h

kgraph$(OBJ)			:	kgraph.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					kgraph.h

kgraph_map_rb$(OBJ)		:	kgraph_map_rb.c				\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					bgraph.h				\
					bgraph_bipart_st.h			\
					kgraph.h				\
					kgraph_map_rb.h

kgraph_map_st$(OBJ)		:	kgraph_map_st.c				\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					bgraph.h				\
					bgraph_bipart_st.h			\
					kgraph.h				\
					kgraph_map_rb.h

mapping$(OBJ)			:	mapping.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					mapping.h

mapping_io$(OBJ)		:	mapping_io.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					mapping_io.h

mesh$(OBJ)			:	mesh.c					\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h

mesh_check$(OBJ)		:	mesh_check.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h

mesh_coarsen$(OBJ)		:	mesh_coarsen.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h

mesh_graph$(OBJ)		:	mesh_graph.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					mesh_graph.h

mesh_induce_sepa$(OBJ)		:	mesh_induce_sepa.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh_induce_sepa.h			\
					mesh.h

mesh_io$(OBJ)			:	mesh_io.c				\
					module.h				\
					common.h				\
					graph.h					\
					graph_io.h				\
					mesh.h					\
					mesh_io.h

mesh_io_habo$(OBJ)		:	mesh_io_habo.c				\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					mesh.h

mesh_io_scot$(OBJ)		:	mesh_io_scot.c				\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					mesh.h

order$(OBJ)			:	order.c					\
					module.h				\
					common.h				\
					graph.h					\
					order.h

order_io$(OBJ)			:	order_io.c				\
					module.h				\
					common.h				\
					graph.h					\
					order.h

order_check$(OBJ)		:	order_check.c				\
					module.h				\
					common.h				\
					graph.h					\
					order.h

order_tree$(OBJ)		:	order_tree.c				\
					module.h				\
					common.h				\
					graph.h					\
					order.h

parser$(OBJ)			:	parser.c				\
					module.h				\
					common.h				\
					parser.h				\
					parser_yy.h

parser_ll.c			:	parser_ll.l				\
					module.h				\
					common.h				\
					parser.h				\
					parser_ll.h				\
					parser_ly.h
					($(LEX) parser_ll.l &&			\
					 $(MV) lex.yy.c parser_ll.c) ||		\
					$(CP) last_resort/parser_ll.c . 

parser_ll$(OBJ)			:	parser_ll.c				\
					parser_ly.h

parser_ly.h			:	parser_yy$(OBJ)

parser_yy.c			:	parser_yy.y				\
					module.h				\
					common.h				\
					parser.h				\
					parser_yy.h
					($(YACC) -d -v parser_yy.y &&		\
					 $(MV) y.tab.c parser_yy.c &&		\
					 $(MV) y.tab.h parser_ly.h) ||		\
					$(CP) last_resort/parser_ly.h last_resort/parser_yy.c .

parser_yy$(OBJ)			:	parser_yy.c

vgraph$(OBJ)			:	vgraph.c				\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h

vgraph_check$(OBJ)		:	vgraph_check.c				\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h

vgraph_separate_es$(OBJ)	:	vgraph_separate_es.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					bgraph.h				\
					bgraph_bipart_st.h			\
					vgraph.h				\
					vgraph_separate_es.h

vgraph_separate_fm$(OBJ)	:	vgraph_separate_fm.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_fm.h

vgraph_separate_gg$(OBJ)	:	vgraph_separate_gg.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_gg.h

vgraph_separate_gp$(OBJ)	:	vgraph_separate_gp.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_gp.h

vgraph_separate_ml$(OBJ)	:	vgraph_separate_ml.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					graph_coarsen.h				\
					vgraph.h				\
					vgraph_separate_ml.h			\
					vgraph_separate_st.h

vgraph_separate_mt$(OBJ)	:	vgraph_separate_mt.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_mt.h

vgraph_separate_th$(OBJ)	:	vgraph_separate_th.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_th.h

vgraph_separate_vw$(OBJ)	:	vgraph_separate_vw.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_vw.h

vgraph_separate_zr$(OBJ)	:	vgraph_separate_zr.c			\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_zr.h

vgraph_separate_st$(OBJ)	:	vgraph_separate_st.c			\
					gain.h					\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					bgraph.h				\
					bgraph_bipart_st.h			\
					vgraph.h				\
					vgraph_separate_fm.h			\
					vgraph_separate_gg.h			\
					vgraph_separate_gp.h			\
					vgraph_separate_ml.h			\
					vgraph_separate_mt.h			\
					vgraph_separate_th.h			\
					vgraph_separate_vw.h			\
					vgraph_separate_zr.h			\
					vgraph_separate_st.h

vgraph_store$(OBJ)		:	vgraph_store.c				\
					module.h				\
					common.h				\
					graph.h					\
					vgraph.h

vmesh$(OBJ)			:	vmesh.c					\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					vmesh.h

vmesh_check$(OBJ)		:	vmesh_check.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					vmesh.h

vmesh_separate_fm$(OBJ)		:	vmesh_separate_fm.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					vmesh.h					\
					vmesh_separate_fm.h

vmesh_separate_gg$(OBJ)		:	vmesh_separate_gg.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					vmesh.h					\
					vmesh_separate_gg.h

vmesh_separate_gr$(OBJ)		:	vmesh_separate_gr.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					vgraph.h				\
					vgraph_separate_st.h			\
					mesh.h					\
					vmesh.h					\
					vmesh_separate_gr.h

vmesh_separate_ml$(OBJ)		:	vmesh_separate_ml.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					mesh.h					\
					mesh_coarsen.h				\
					vmesh.h					\
					vmesh_separate_ml.h			\
					vmesh_separate_st.h

vmesh_separate_zr$(OBJ)		:	vmesh_separate_zr.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					vmesh.h					\
					vmesh_separate_zr.h

vmesh_separate_st$(OBJ)		:	vmesh_separate_st.c			\
					gain.h					\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					mesh.h					\
					mesh_coarsen.h				\
					vmesh.h					\
					vmesh_separate_fm.h			\
					vmesh_separate_gg.h			\
					vmesh_separate_gr.h			\
					vmesh_separate_ml.h			\
					vmesh_separate_zr.h			\
					vmesh_separate_st.h

vmesh_store$(OBJ)		:	vmesh_store.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					vmesh.h

dummysizes$(EXE)		:	dummysizes.c				\
					module.h				\
					common.h				\
					graph.h					\
					geom.h					\
					arch.h					\
					mesh.h					\
					mapping.h				\
					order.h					\
					parser.h

scotch.h			:	dummysizes$(EXE)			\
					library.h
					./dummysizes library.h scotch.h

scotchf.h			:	dummysizes$(EXE) 			\
					libraryf.h
					./dummysizes libraryf.h scotchf.h

library_arch$(OBJ)		:	library_arch.c				\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					arch_cmplt.h				\
					scotch.h

library_arch_f$(OBJ)		:	library_arch_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					scotch.h

library_arch_build$(OBJ)	:	library_arch_build.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					arch_build.h				\
					mapping.h				\
					bgraph.h				\
					bgraph_bipart_st.h			\
					scotch.h

library_arch_bulid_f$(OBJ)	:	library_arch_build_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					arch.h					\
					scotch.h

library_error$(OBJ)		:	library_error.c				\
					module.h				\
					common.h				\
					scotch.h

library_errcom$(OBJ)		:	library_errcom.c			\
					module.h				\
					common.h				\
					scotch.h

library_geom$(OBJ)		:	library_geom.c				\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_geom_f$(OBJ)		:	library_geom_f.c			\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph_io_chac$(OBJ)	:	library_graph_io_chac.c			\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph_io_chac_f$(OBJ)	:	library_graph_io_chac_f.c		\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph_io_habo$(OBJ)	:	library_graph_io_habo.c			\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph_io_habo_f$(OBJ)	:	library_graph_io_habo_f.c		\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph_io_scot$(OBJ)	:	library_graph_io_scot.c			\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph_io_scot_f$(OBJ)	:	library_graph_io_scot_f.c		\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					scotch.h

library_graph$(OBJ)		:	library_graph.c				\
					module.h				\
					common.h				\
					graph.h					\
					graph_io.h				\
					scotch.h

library_graph_f$(OBJ)		:	library_graph_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					graph_io.h				\
					scotch.h

library_graph_base$(OBJ)	:	library_graph_base.c			\
					module.h				\
					common.h				\
					graph.h					\
					scotch.h

library_graph_base_f$(OBJ)	:	library_graph_base_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					scotch.h

library_graph_check$(OBJ)	:	library_graph_check.c			\
					module.h				\
					common.h				\
					graph.h					\
					scotch.h

library_graph_check_f$(OBJ)	:	library_graph_check_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					scotch.h

library_graph_map$(OBJ)		:	library_graph_map.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					kgraph.h				\
					kgraph_map_st.h				\
					scotch.h

library_graph_map_f$(OBJ)	:	library_graph_map_f.c			\
					module.h				\
					common.h				\
					scotch.h

library_graph_map_view$(OBJ)	:	library_graph_map_view.c		\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					arch.h					\
					mapping.h				\
					kgraph.h				\
					kgraph_map_st.h				\
					scotch.h

library_graph_map_view_f$(OBJ)	:	library_graph_map_view_f.c		\
					module.h				\
					common.h				\
					scotch.h

library_graph_order$(OBJ)	:	library_graph_order.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					order.h					\
					hgraph.h				\
					hgraph_order_st.h			\
					library_order.h				\
					scotch.h

library_graph_order_f$(OBJ)	:	library_graph_order_f.c			\
					module.h				\
					common.h				\
					scotch.h

library_mesh$(OBJ)		:	library_mesh.c				\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_f$(OBJ)		:	library_mesh_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_graph$(OBJ)	:	library_mesh_graph.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_graph_f$(OBJ)	:	library_mesh_graph_f.c			\
					module.h				\
					common.h				\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_io_habo$(OBJ)	:	library_mesh_io_habo.c			\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_io_habo_f$(OBJ)	:	library_mesh_io_habo_f.c		\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_io_scot$(OBJ)	:	library_mesh_io_scot.c			\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_io_scot_f$(OBJ)	:	library_mesh_io_scot_f.c		\
					module.h				\
					common.h				\
					geom.h					\
					graph.h					\
					mesh.h					\
					scotch.h

library_mesh_order$(OBJ)	:	library_mesh_order.c			\
					module.h				\
					common.h				\
					parser.h				\
					graph.h					\
					mesh.h					\
					hmesh.h					\
					order.h					\
					hmesh_order_st.h			\
					scotch.h			\
					library_order.h

library_mesh_order$(OBJ)_f	:	library_mesh_order_f.c			\
					module.h				\
					common.h				\
					scotch.h

library_parser$(OBJ)		:	library_parser.c			\
					module.h				\
					common.h				\
					parser.h				\
					scotch.h

library_parser_f$(OBJ)		:	library_parser_f.c			\
					module.h				\
					common.h				\
					scotch.h

library_random$(OBJ)		:	library_random.c			\
					module.h				\
					common.h				\
					scotch.h

library_random_f$(OBJ)		:	library_random_f.c			\
					module.h				\
					common.h				\
					scotch.h

libscotch$(LIB)			:	arch$(OBJ)				\
					arch_build$(OBJ)			\
					arch_cmplt$(OBJ)			\
					arch_deco$(OBJ)				\
					arch_hcub$(OBJ)				\
					arch_mesh$(OBJ)				\
					arch_tleaf$(OBJ)			\
					arch_torus$(OBJ)			\
					arch_vcmplt$(OBJ)			\
					arch_vhcub$(OBJ)			\
					bgraph$(OBJ)				\
					bgraph_bipart_ex$(OBJ)			\
					bgraph_bipart_fm$(OBJ)			\
					bgraph_bipart_gg$(OBJ)			\
					bgraph_bipart_gp$(OBJ)			\
					bgraph_bipart_ml$(OBJ)			\
					bgraph_bipart_st$(OBJ)			\
					bgraph_bipart_zr$(OBJ)			\
					bgraph_check$(OBJ)			\
					bgraph_store$(OBJ)			\
					common$(OBJ)				\
					common_integer$(OBJ)			\
					common_memory$(OBJ)			\
					gain$(OBJ)				\
					geom$(OBJ)				\
					graph$(OBJ)				\
					graph_base$(OBJ)			\
					graph_check$(OBJ)			\
					graph_coarsen$(OBJ)			\
					graph_induce$(OBJ)			\
					graph_io$(OBJ)				\
					graph_io_chac$(OBJ)			\
					graph_io_habo$(OBJ)			\
					graph_io_scot$(OBJ)			\
					graph_list$(OBJ)			\
					hall_order_hd$(OBJ)			\
					hall_order_hf$(OBJ)			\
					hall_order_hx$(OBJ)			\
					hgraph$(OBJ)				\
					hgraph_induce$(OBJ)			\
					hgraph_order_bl$(OBJ)			\
					hgraph_order_cp$(OBJ)			\
					hgraph_order_gp$(OBJ)			\
					hgraph_order_hd$(OBJ)			\
					hgraph_order_hf$(OBJ)			\
					hgraph_order_hx$(OBJ)			\
					hgraph_order_nd$(OBJ)			\
					hgraph_order_si$(OBJ)			\
					hgraph_order_st$(OBJ)			\
					hmesh$(OBJ)				\
					hmesh_check$(OBJ)			\
					hmesh_hgraph$(OBJ)			\
					hmesh_induce$(OBJ)			\
					hmesh_mesh$(OBJ)			\
					hmesh_order_bl$(OBJ)			\
					hmesh_order_cp$(OBJ)			\
					hmesh_order_gr$(OBJ)			\
					hmesh_order_gp$(OBJ)			\
					hmesh_order_hd$(OBJ)			\
					hmesh_order_hf$(OBJ)			\
					hmesh_order_hx$(OBJ)			\
					hmesh_order_nd$(OBJ)			\
					hmesh_order_si$(OBJ)			\
					hmesh_order_st$(OBJ)			\
					kgraph$(OBJ)				\
					kgraph_map_rb$(OBJ)			\
					kgraph_map_st$(OBJ)			\
					library_arch$(OBJ)			\
					library_arch_f$(OBJ)			\
					library_arch_build$(OBJ)		\
					library_arch_build_f$(OBJ)		\
					library_geom$(OBJ)			\
					library_geom_f$(OBJ)			\
					library_graph$(OBJ)			\
					library_graph_f$(OBJ)			\
					library_graph_base$(OBJ)		\
					library_graph_base_f$(OBJ)		\
					library_graph_check$(OBJ)		\
					library_graph_check_f$(OBJ)		\
					library_graph_io_chac$(OBJ)		\
					library_graph_io_chac_f$(OBJ)		\
					library_graph_io_habo$(OBJ)		\
					library_graph_io_habo_f$(OBJ)		\
					library_graph_io_scot$(OBJ)		\
					library_graph_io_scot_f$(OBJ)		\
					library_graph_map$(OBJ)			\
					library_graph_map_f$(OBJ)		\
					library_graph_map_view$(OBJ)		\
					library_graph_map_view_f$(OBJ)		\
					library_graph_order$(OBJ)		\
					library_graph_order_f$(OBJ)		\
					library_mesh$(OBJ)			\
					library_mesh_f$(OBJ)			\
					library_mesh_graph$(OBJ)		\
					library_mesh_graph_f$(OBJ)		\
					library_mesh_io_habo$(OBJ)		\
					library_mesh_io_habo_f$(OBJ)		\
					library_mesh_io_scot$(OBJ)		\
					library_mesh_io_scot_f$(OBJ)		\
					library_mesh_order$(OBJ)		\
					library_mesh_order_f$(OBJ)		\
					library_parser$(OBJ)			\
					library_parser_f$(OBJ)			\
					library_random$(OBJ)			\
					library_random_f$(OBJ)			\
					mapping$(OBJ)				\
					mapping_io$(OBJ)			\
					mesh$(OBJ)				\
					mesh_check$(OBJ)			\
					mesh_coarsen$(OBJ)			\
					mesh_graph$(OBJ)			\
					mesh_io$(OBJ)				\
					mesh_io_habo$(OBJ)			\
					mesh_io_scot$(OBJ)			\
					mesh_induce_sepa$(OBJ)			\
					order$(OBJ)				\
					order_check$(OBJ)			\
					order_io$(OBJ)				\
					parser$(OBJ)				\
					parser_yy$(OBJ)				\
					parser_ll$(OBJ)				\
					vgraph$(OBJ)				\
					vgraph_check$(OBJ)			\
					vgraph_store$(OBJ)			\
					vgraph_separate_es$(OBJ)		\
					vgraph_separate_fm$(OBJ)		\
					vgraph_separate_gg$(OBJ)		\
					vgraph_separate_gp$(OBJ)		\
					vgraph_separate_ml$(OBJ)		\
					vgraph_separate_st$(OBJ)		\
					vgraph_separate_th$(OBJ)		\
					vgraph_separate_vw$(OBJ)		\
					vgraph_separate_zr$(OBJ)		\
					vmesh$(OBJ)				\
					vmesh_check$(OBJ)			\
					vmesh_separate_fm$(OBJ)			\
					vmesh_separate_gg$(OBJ)			\
					vmesh_separate_gr$(OBJ)			\
					vmesh_separate_ml$(OBJ)			\
					vmesh_separate_zr$(OBJ)			\
					vmesh_separate_st$(OBJ)			\
					vmesh_store$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(?)
					-$(RANLIB) $(@)

libscotcherr$(LIB)		:	library_error$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(?)
					-$(RANLIB) $(@)

libscotcherrcom$(LIB)		:	library_errcom$(OBJ)
					$(AR) $(ARFLAGS) $(@) $(?)
					-$(RANLIB) $(@)
