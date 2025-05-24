enum {
  BUFFER_SIZE = sizeof(union Screen),
  PAGE_SIZE = 4096,
  PAGES_FOR_BUFFER = (BUFFER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE
};
