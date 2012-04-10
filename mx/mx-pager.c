/*
 * mx-pager.c: A container that allows you to display several pages of widgets
 *
 * Copyright 2012 Intel Corporation.
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
 * Written by: Danielle Madeley <danielle.madeley@collabora.co.uk>
 */

#include "mx-pager.h"

#define PAGER_WIDTH 20.

static void clutter_container_iface_init (gpointer, gpointer);

G_DEFINE_TYPE_WITH_CODE (MxPager, mx_pager, MX_TYPE_STACK,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
                                                clutter_container_iface_init)
                        )

struct _MxPagerPrivate
{
  GList *pages;
  GList *current_page;
};

/**
 * mx_pager_add_internal_actor:
 *
 * Chains up to the internal add() call of the #MxStack.
 */
static void
mx_pager_add_internal_actor (MxPager      *self,
                             ClutterActor *child)
{
  ClutterContainerIface *parent_iface =
    g_type_interface_peek_parent (CLUTTER_CONTAINER_GET_IFACE (self));

  parent_iface->add ((ClutterContainer *) self, child);
}

/**
 * mx_pager_change_page:
 * @self:
 * @new_page: pointer to the new page
 * @animate: whether to animate the transition
 *
 * Changes the currently visible page.
 */
static void
mx_pager_change_page (MxPager *self,
                      GList   *new_page,
                      gboolean animate)
{
  if (new_page == self->priv->current_page)
    return;

  g_debug ("Change page!");

  if (self->priv->current_page != NULL)
    {
      ClutterActor *page = self->priv->current_page->data;

      clutter_actor_set_opacity (page, 0x0);
    }

  if (new_page != NULL)
    {
      ClutterActor *page = new_page->data;

      clutter_actor_set_opacity (page, 0xff);
    }

  self->priv->current_page = new_page;
}

static void
mx_pager_class_init (MxPagerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (MxPagerPrivate));
}


static void
mx_pager_init (MxPager *self)
{
  ClutterActor *prevbox, *nextbox;

  const ClutterColor transparent = { 0xf0, 0x00, 0x00, 0xf0 };

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MX_TYPE_PAGER,
                                            MxPagerPrivate);

  clutter_actor_set_clip_to_allocation (CLUTTER_ACTOR (self), TRUE);

  prevbox = clutter_rectangle_new_with_color (&transparent);

  clutter_actor_set_width (prevbox, PAGER_WIDTH);
  clutter_actor_set_reactive (prevbox, TRUE);
  mx_pager_add_internal_actor (self, prevbox);
  mx_stack_child_set_x_fill (MX_STACK (self), prevbox, FALSE);
  mx_stack_child_set_x_align (MX_STACK (self), prevbox, MX_ALIGN_START);

  g_signal_connect_swapped (prevbox, "button-press-event",
      G_CALLBACK (mx_pager_previous), self);

  nextbox = clutter_rectangle_new_with_color (&transparent);

  clutter_actor_set_width (nextbox, PAGER_WIDTH);
  clutter_actor_set_reactive (nextbox, TRUE);
  mx_pager_add_internal_actor (self, nextbox);
  mx_stack_child_set_x_fill (MX_STACK (self), nextbox, FALSE);
  mx_stack_child_set_x_align (MX_STACK (self), nextbox, MX_ALIGN_END);

  g_signal_connect_swapped (nextbox, "button-press-event",
      G_CALLBACK (mx_pager_next), self);
}

static void
mx_pager_add (ClutterContainer *self,
              ClutterActor     *child)
{
  MxPagerPrivate *priv = MX_PAGER (self)->priv;

  priv->pages = g_list_append (priv->pages, child);

  clutter_actor_set_opacity (child, 0x0);

  mx_pager_add_internal_actor ((MxPager *) self, child);
  clutter_actor_lower_bottom (child);

  if (priv->current_page == NULL)
    mx_pager_change_page (MX_PAGER (self), priv->pages, FALSE);
}

static void
mx_pager_remove (ClutterContainer *self,
                 ClutterActor     *child)
{
  MxPagerPrivate *priv = MX_PAGER (self)->priv;
  ClutterContainerIface *parent_iface =
    g_type_interface_peek_parent (CLUTTER_CONTAINER_GET_IFACE (self));
  GList *l;

  l = g_list_find (priv->pages, child);

  if (l == NULL)
    {
      g_warning (G_STRLOC ": Actor of type '%s' is not a child of container "
                 "of type '%s'",
                 G_OBJECT_TYPE_NAME (child),
                 G_OBJECT_TYPE_NAME (self));
      return;
    }

  if (priv->current_page == l)
    {
      /* change the current page */
      if (l->next != NULL)
        mx_pager_change_page (MX_PAGER (self), l->next, FALSE);
      else
        mx_pager_change_page (MX_PAGER (self), priv->pages, FALSE);
    }

  priv->pages = g_list_delete_link (priv->pages, l);

  parent_iface->remove (self, child);
}

static void
mx_pager_foreach (ClutterContainer *self,
                  ClutterCallback   callback,
                  gpointer          user_data)
{
  MxPagerPrivate *priv = MX_PAGER (self)->priv;

  g_list_foreach (priv->pages, (GFunc) callback, user_data);
}

static void
clutter_container_iface_init (gpointer g_iface,
                              gpointer iface_data)
{
  ClutterContainerIface *iface = g_iface;

#define IMPLEMENT(x) \
  iface->x = mx_pager_##x;

  IMPLEMENT (add);
  IMPLEMENT (remove);
  IMPLEMENT (foreach);

#undef IMPLEMENT
}

/**
 * mx_pager_new:
 *
 * Returns: (transfer full) (type Mx.Pager): a new #MxPager
 */
ClutterActor *
mx_pager_new (void)
{
  return g_object_new (MX_TYPE_PAGER, NULL);
}

/**
 * mx_pager_next:
 * @self: a #MxPager
 *
 * Move to the next page.
 */
void
mx_pager_next (MxPager *self)
{
  g_return_if_fail (MX_IS_PAGER (self));
  g_return_if_fail (self->priv->current_page != NULL);

  if (self->priv->current_page->next == NULL)
    return;

  g_debug ("next page");

  mx_pager_change_page (self, self->priv->current_page->next, TRUE);
}

/**
 * mx_pager_previous:
 * @self: a #MxPager
 *
 * Move to the previous page.
 */
void
mx_pager_previous (MxPager *self)
{
  g_return_if_fail (MX_IS_PAGER (self));
  g_return_if_fail (self->priv->current_page != NULL);

  if (self->priv->current_page->prev == NULL)
    return;

  g_debug ("prev page");

  mx_pager_change_page (self, self->priv->current_page->prev, TRUE);
}

/**
 * mx_pager_set_current_page:
 * @self: a #MxPager
 * @page: the page to move to
 * @animate: whether to animate the move between pages
 *
 * Move to @page.
 */
void
mx_pager_set_current_page (MxPager *self,
                           guint    page,
                           gboolean animate)
{
  GList *page_l;

  g_return_if_fail (MX_IS_PAGER (self));

  page_l = g_list_nth (self->priv->pages, page);

  g_return_if_fail (page_l != NULL);

  mx_pager_change_page (self, page_l, animate);
}

/**
 * mx_pager_get_current_page:
 * @self: a #MxPager
 *
 * Returns: the current page number
 */
guint
mx_pager_get_current_page (MxPager *self)
{
  int pos;

  g_return_val_if_fail (MX_IS_PAGER (self), 0);

  pos = g_list_position (self->priv->pages, self->priv->current_page);

  g_return_val_if_fail (pos >= 0, 0);

  return pos;
}

/**
 * mx_pager_set_current_page_by_actor:
 * @self: a #MxPager
 * @actor: the actor of the page to move to
 * @animate: whether to animate the move between pages
 *
 * Move to the page containing @actor.
 */
void
mx_pager_set_current_page_by_actor (MxPager      *self,
                                    ClutterActor *actor,
                                    gboolean      animate)
{
  GList *page_l;

  g_return_if_fail (MX_IS_PAGER (self));

  page_l = g_list_find (self->priv->pages, actor);

  g_return_if_fail (page_l != NULL);

  mx_pager_change_page (self, page_l, animate);
}

/**
 * mx_pager_get_current_page_actor:
 * @self: a #MxPager
 *
 * Returns: (transfer none): the #ClutterActor on the current page
 */
ClutterActor *
mx_pager_get_current_page_actor (MxPager *self)
{
  g_return_val_if_fail (MX_IS_PAGER (self), NULL);

  return CLUTTER_ACTOR (self->priv->current_page->data);
}

/**
 * mx_pager_get_actor_for_page:
 * @self: a #MxPager
 * @page: a page number
 *
 * Returns: (transfer none): the #ClutterActor for @page
 */
ClutterActor *
mx_pager_get_actor_for_page (MxPager *self,
                             guint    page)
{
  g_return_val_if_fail (MX_IS_PAGER (self), NULL);

  return CLUTTER_ACTOR (g_list_nth_data (self->priv->pages, page));
}