#include "user_info.h"
#include "../AppState.h"
#include "../gui_components.h"

UserInfo::UserInfo() : t_username(username_text), t_bio(bio_text), t_state(state_text) {
  t_bio.allowGrow = true;
  t_bio.growDown = true;
  t_bio.color = vec4f(1, 1, 1, 0.8);
  t_bio.scale = 0.7;
   t_state.allowGrow = true;
  t_state.growDown = true;
  t_state.color = vec4f(0.75, 0.75, 0.75, 1);
  t_state.scale = 0.6;
}

bool UserInfo::canFocus() {
  return false;
}
void UserInfo::onFocus(bool focus) {}
void UserInfo::render(float x, float y, float w, float h) {
  auto abs = AppState::gState->getPositionAbsolute(x, y, 0, 0);
  x = abs.x;
  y = abs.y;

  Box::render(x, y, w, -h, vec4f(0, 0, 0, 1));

  auto atlas_height = AppState::gState->atlas.effective_atlas_height;
  float xOffset = 0;
  if (user_image != nullptr) {
    float imgWidth = w * 0.3;
    float imgScale = imgWidth / user_image->width;
    user_image->render(x, -y, imgScale);
    xOffset += imgWidth + 15;
  }
  y -= atlas_height;
  t_username.render(x + xOffset, y, w - xOffset, atlas_height + 5);
  y -= atlas_height;
    if(presence.emote_id.length()) {
    auto* res = AppState::gState->client->image_cache.getEmote(presence.emote_id);
    if(res) {
          float scale = (float)50 / res->height;
           res->render(x + xOffset,
                    -y - (atlas_height * 0.6),
                    scale);
         y -= (50 + 15);
    }

  }
  if(state_text.data.size()) {
   t_state.render(x + xOffset, y, w - xOffset, atlas_height + 5);

      y -= (t_state.computeHeight(w - xOffset) * atlas_height);
  }
  t_bio.render(x + xOffset, y - 5, w - xOffset,
               atlas_height + 5);
}
void UserInfo::initFrom(DiscordRichUser user, DiscordPresence presence) {
  this->user = user;
  this->presence = presence;
  if (user_image != nullptr) {
    delete user_image;
    user_image = nullptr;
  }
  username_text.setData(user.user.username + "#" + user.user.discriminator);
  bio_text.setData(user.user.bio);
  if(presence.utf_emote.length())
      state_text.setData(presence.utf_emote+ presence.custom_status);
    else
  state_text.setData(presence.custom_status);
  if (user.user.avatar.length()) {
    auto* t = this;
    std::string user_image_url = "https://cdn.discordapp.com/avatars/" +
                                 user.user.id + "/" + user.user.avatar +
                                 ".webp?size=240";
    AppState::gState->client->image_cache.fetchImage(
        user_image_url, t, [t](bool success, ImageCacheEntry* image) {
          if (!success) {
            return;
          }
          AppState::gState->components->runLater(
              new std::function([t, image]() {
                Image* img = new Image();
                img->init_from_mem_webp(image->data);
                t->user_image = img;
              }));
        });
  }
}