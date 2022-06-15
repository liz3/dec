#ifndef DEC_MESSAGE_LIST
#define DEC_MESSAGE_LIST
#include "../rendering/box.h"
#include "../rendering/image.h"
#include "../rendering/text.h"
#include <mutex>

#include "component.h"

#include "../rendering/text_box.h"
class MessageHolder;

class EmbedRender {
 public:
  DiscordMessageEmbed embed;
  Image* image = nullptr;
  TextWithState title, description, footer;
  TextBox t_box, d_box, f_box;
  float height = 0.0;
  float ah;
  float getHeight(float w, float atlas_height);
  EmbedRender(DiscordMessageEmbed embed);
  ~EmbedRender();
  void render(float x, float y, float w);
};
class RenderMessage {
 public:
  TextWithState text;
  TextBox box;
  TextWithState title;
  TextBox title_box;
  MessageHolder* m_holder;
  RenderMessage(MessageHolder* holder);
  std::vector<Image*> images;
  std::vector<EmbedRender*> embeds;
  ~RenderMessage();
  int getHeight(float w, float atlas_height);
  void render(float x, float y, float w, bool selected = false);
  float atlas_height;
  float start = 0.0;
  float height = 0.0;
  float content_height = 0.0;
  std::mutex mtx;
  void fetchImage(std::string url);
};

class MessageList : public Component {
 public:
  std::vector<RenderMessage*> messages;
  float height = 0.0;
  float currentOffset = 0.0;
  float availableHeight = 0.0;
  float width = 500;
  int32_t selected_index = -1;
  RenderMessage* addContent(MessageHolder* h, bool prepend = false);
  std::vector<MessageHolder*>* msg_load_ptr = nullptr;
  bool hasFocus = false;
  void clearList();
  bool canFocus() override;
  void onFocus(bool focus) override;
  void render(float x, float y, float w, float h) override;
  void recomputeHeights();
  void setWidth(float width);
  void setAvailableHeight(float y);
  void changeScroll(float offset);
  void scrollEnd();
  void selectIndex(int32_t diff);
  void updateEntry(MessageHolder*);
  void removeEntry(MessageHolder*);
};
#endif