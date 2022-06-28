#include "message_state.h"

void MessageState::add_message(DiscordMessagePayload message,
                               DiscordChannelPayload& channel) {
  if (!state.count(channel.id) && channel.type != DmText) {
    return;
  }
  if (!state.count(channel.id)) {
    ChannelState channel_state;
    state[channel.id] = channel_state;
  }
  ChannelState& channel_state = state[channel.id];
  MessageHolder* holder = new MessageHolder(message);

  channel_state.messages.push_back(holder);
  message_index[message.id] = holder;
}
void MessageState::remove_message(std::string id) {
  if (!message_index.count(id))
    return;
  MessageHolder* h = message_index[id];
  ChannelState& channel_state = state[h->message.channel_id];
  int32_t index = -1;
  for (int i = 0; i < channel_state.messages.size(); ++i) {
    if (channel_state.messages[i] == h) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    // wtf?
    return;
  }
  channel_state.messages.erase(channel_state.messages.begin() + index);
  message_index.erase(id);
  delete h;
}
// important! need to take care incase message_list has refs to this!!!!!!
void MessageState::remove_channel(std::string id) {
  if (!state.count(id))
    return;
  ChannelState& channel = state[id];
  for (auto* message : channel.messages) {
    message_index.erase(message->id);
    delete message;
  }
  state.erase(id);
}
MessageHolder::MessageHolder(DiscordMessagePayload p) : message(p) {
  id = message.id;
}
void MessageState::load_channel(std::vector<DiscordMessagePayload>& messages,
                                DiscordChannelPayload& channel) {
  if (state.count(channel.id) && (&state[channel.id])->loaded) {
    return;
  }
  if (!state.count(channel.id)) {
    ChannelState channel_state;
    state[channel.id] = channel_state;
  }
  ChannelState& channel_state = state[channel.id];

  for (auto msg : messages) {
    if(msg.content.find("dec") == 0 && (msg.content[3] == '0' || msg.content[3] == '1' || msg.content[3] == '2' || msg.content[3] == '3'))
      continue;
    MessageHolder* holder = new MessageHolder(msg);
    channel_state.messages.insert(channel_state.messages.begin(), holder);
    message_index[msg.id] = holder;
  }

  channel_state.loaded = true;
}
std::vector<MessageHolder*> MessageState::prepend_messages(
    std::vector<DiscordMessagePayload>& messages,
    DiscordChannelPayload& channel) {
  std::vector<MessageHolder*> ret;
  if (!state.count(channel.id)) {
    return ret;
  }
  ChannelState& channel_state = state[channel.id];
  for (auto msg : messages) {
        if(msg.content.find("dec") == 0 && (msg.content[3] == '0' || msg.content[3] == '1' || msg.content[3] == '2' || msg.content[3] == '3'))
      continue;
    MessageHolder* holder = new MessageHolder(msg);
    channel_state.messages.insert(channel_state.messages.begin(), holder);
    ret.push_back(holder);
    message_index[msg.id] = holder;
  }
  return ret;
}