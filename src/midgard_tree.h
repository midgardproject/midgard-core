
#ifndef MIDGARD_TREE_H
#define MIDGARD_TREE_H

gboolean _midgard_tree_exists(MidgardConnection *mgd,
				const gchar *table, const gchar *upfield,
				gint root, gint id);

guint *_midgard_tree_ids(MidgardConnection *mgd, 
				MidgardObjectClass *klass, guint startid);
#endif /* MIDGARD_TREE_H */
