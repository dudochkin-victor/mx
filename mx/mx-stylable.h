/*
 * mx-stylable.h: Interface for stylable objects
 *
 * Copyright 2008, 2009 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * Boston, MA 02111-1307, USA.
 *
 * Written by: Emmanuele Bassi <ebassi@openedhand.com>
 *             Thomas Wood <thomas@linux.intel.com>
 *
 */

#if !defined(MX_H_INSIDE) && !defined(MX_COMPILATION)
#error "Only <mx/mx.h> can be included directly.h"
#endif

#ifndef __MX_STYLABLE_H__
#define __MX_STYLABLE_H__

#include <glib-object.h>
#include <mx/mx-style.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define MX_TYPE_STYLABLE              (mx_stylable_get_type ())
#define MX_STYLABLE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MX_TYPE_STYLABLE, MxStylable))
#define MX_IS_STYLABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MX_TYPE_STYLABLE))
#define MX_STYLABLE_IFACE(iface)      (G_TYPE_CHECK_CLASS_CAST ((iface), MX_TYPE_STYLABLE, MxStylableIface))
#define MX_IS_STYLABLE_IFACE(iface)   (G_TYPE_CHECK_CLASS_TYPE ((iface), MX_TYPE_STYLABLE))
#define MX_STYLABLE_GET_IFACE(obj)    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), MX_TYPE_STYLABLE, MxStylableIface))

/* MxStylableIface is defined in mx-style.h */

struct _MxStylableIface
{
  GTypeInterface g_iface;

  /* virtual functions */
  MxStyle *  (* get_style) (MxStylable *stylable);
  void       (* set_style) (MxStylable *stylable,
                            MxStyle    *style);

  G_CONST_RETURN gchar* (* get_style_class) (MxStylable  *stylable);
  void                  (* set_style_class) (MxStylable  *stylable,
                                             const gchar *style_class);

  G_CONST_RETURN gchar* (* get_style_pseudo_class) (MxStylable  *stylable);
  void                  (* set_style_pseudo_class) (MxStylable  *stylable,
                                                    const gchar *style_class);

  /* context virtual functions */

  /* signals, not vfuncs */
  void (* style_notify)     (MxStylable *stylable,
                             GParamSpec *pspec);
  void (* style_changed)    (MxStylable *stylable);

  void (* stylable_changed) (MxStylable *stylable);
};

GType        mx_stylable_get_type               (void) G_GNUC_CONST;

void         mx_stylable_iface_install_property (MxStylableIface *iface,
                                                 GType              owner_type,
                                                 GParamSpec        *pspec);

void         mx_stylable_freeze_notify          (MxStylable      *stylable);
void         mx_stylable_notify                 (MxStylable      *stylable,
                                                 const gchar       *property_name);
void         mx_stylable_thaw_notify            (MxStylable      *stylable);
GParamSpec **mx_stylable_list_properties        (MxStylable      *stylable,
                                                 guint             *n_props);
GParamSpec * mx_stylable_find_property          (MxStylable      *stylable,
                                                 const gchar       *property_name);
void         mx_stylable_set_style              (MxStylable      *stylable,
                                                 MxStyle         *style);
MxStyle *    mx_stylable_get_style              (MxStylable      *stylable);

void         mx_stylable_get                    (MxStylable      *stylable,
                                                 const gchar       *first_property_name,
                                                 ...) G_GNUC_NULL_TERMINATED;
void         mx_stylable_get_property           (MxStylable      *stylable,
                                                 const gchar       *property_name,
                                                 GValue            *value);
gboolean     mx_stylable_get_default_value      (MxStylable      *stylable,
                                                 const gchar       *property_name,
                                                 GValue            *value_out);

void mx_stylable_changed (MxStylable *stylable);

G_CONST_RETURN gchar* mx_stylable_get_style_class (MxStylable  *stylable);
void                  mx_stylable_set_style_class (MxStylable  *stylable,
                                                   const gchar *style_class);

G_CONST_RETURN gchar* mx_stylable_get_style_pseudo_class (MxStylable  *stylable);
void                  mx_stylable_set_style_pseudo_class (MxStylable  *stylable,
                                                          const gchar *pseudo_class);

/* utilities */
void mx_stylable_apply_clutter_text_attributes (MxStylable  *stylable,
                                                ClutterText *text);

G_END_DECLS

#endif /* __MX_STYLABLE_H__ */
